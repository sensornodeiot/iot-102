#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <WiFiManager.h>  // Include WiFiManager library

// ThingsBoard credentials
#define TOKEN "lmi9i60shg21zjtopfew"
char thingsboardServer[] = "159.223.80.40";

// Pin definitions
#define DHT_PIN 4
#define DHT_TYPE DHT11
#define LED_PIN 12  // Change to GPIO2 for external LED
#define BUTTON_PIN 13  // Push button pin

// 7-Segment Display Pins
int segmentPins[] = {23, 18, 25, 27, 14, 22, 33, 26};
int digitPins[] = {2, 21, 19, 32};
const byte numbers[10][8] = {
    {1, 1, 1, 1, 1, 1, 0, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1, 0}, // 2
    {1, 1, 1, 1, 0, 0, 1, 0}, // 3
    {0, 1, 1, 0, 0, 1, 1, 0}, // 4
    {1, 0, 1, 1, 0, 1, 1, 0}, // 5
    {1, 0, 1, 1, 1, 1, 1, 0}, // 6
    {1, 1, 1, 0, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1, 0}, // 8
    {1, 1, 1, 1, 0, 1, 1, 0}  // 9
};

int displayDigits[4] = {0, 0, 0, 0};
bool showTemperature = true;
unsigned long lastSwitchTime = 0;

WiFiClient wifiClient;
PubSubClient client(wifiClient);
DHT dht(DHT_PIN, DHT_TYPE);

unsigned long previousMillis = 0;
const unsigned long interval = 5000;  // Interval to send DHT data
bool ledState = false;  // Track the LED state
bool lastButtonState = LOW;  // Last button state
bool currentButtonState = LOW;  // Current button state

// Function prototypes
void reconnect();
void on_message(const char* topic, byte* payload, unsigned int length);

void updateDisplayDigits(int value) {
    displayDigits[0] = (value / 1000) % 10;
    displayDigits[1] = (value / 100) % 10;
    displayDigits[2] = (value / 10) % 10;
    displayDigits[3] = value % 10;
}

void displayDigit(int num) {
    for (int i = 0; i < 8; i++) {
        digitalWrite(segmentPins[i], numbers[num][i] ? HIGH : LOW);
    }
}

void setup() {
    Serial.begin(9600);

    // Initialize LED pin
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);  // Ensure LED is off initially

    // Initialize button pin
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Initialize 7-segment display pins
    for (int i = 0; i < 8; i++) pinMode(segmentPins[i], OUTPUT);
    for (int i = 0; i < 4; i++) pinMode(digitPins[i], OUTPUT);

    // Initialize WiFiManager
    WiFiManager wifiManager;
    if (!wifiManager.autoConnect("ESP32-ConfigAP")) {
        Serial.println("Failed to connect to WiFi, restarting...");
        ESP.restart();
    }

    // Initialize MQTT client
    client.setServer(thingsboardServer, 1883);
    client.setCallback(on_message);

    // Initialize DHT sensor
    dht.begin();
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  if (millis() - lastSwitchTime >= 5000) {
      lastSwitchTime = millis();
      showTemperature = !showTemperature;

      float temp = dht.readTemperature();
      float hum = dht.readHumidity();

      if (isnan(temp) || isnan(hum)) {
          Serial.println("Failed to read from DHT sensor!");
          return;
      }

      // Convert float to int (truncating the decimal part)
      int value = showTemperature ? (int)temp : (int)hum;
      updateDisplayDigits(value);
      Serial.print("Displaying: ");
      Serial.println(value);
  }

  // Update the 7-segment display with the correct digits
  for (int i = 0; i < 4; i++) {
      digitalWrite(digitPins[i], LOW);
      displayDigit(displayDigits[i]);
      delay(5);
      digitalWrite(digitPins[i], HIGH);
  }

  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (currentButtonState == LOW && lastButtonState == HIGH) {
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState ? HIGH : LOW);

      String payload = "{\"ledState\":" + String(ledState) + "}";  // Send LED state to ThingsBoard
      client.publish("SSN/Project/IoT102/test/telemetry", payload.c_str());
  }
  lastButtonState = currentButtonState;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 5000) {
      float temperature = dht.readTemperature();
      float humidity = dht.readHumidity();

      if (!isnan(temperature) && !isnan(humidity)) {
          // Send temperature and humidity to ThingsBoard
          String payload = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + ",\"ledState\":" + String(ledState) + "}";
          client.publish("SSN/Project/IoT102/test/telemetry", payload.c_str());
      }
      previousMillis = currentMillis;
  }
}


// Callback for handling RPC messages
void on_message(const char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.println("]");

    // Convert payload to a string
    char message[length + 1];
    strncpy(message, (char*)payload, length);
    message[length] = '\0';

    // Print the payload for debugging
    Serial.print("Payload: ");
    Serial.println(message);

    // Parse the JSON payload
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, message);

    // Check for parsing errors
    if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return;
    }

    // Extract method and params from the JSON payload
    const char* method = doc["method"];
    bool params = doc["params"];

    // Handle the RPC command
    if (strcmp(method, "setValue") == 0) {
        // Update the LED state
        ledState = params;
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);  // Control LED on GPIO2
        Serial.println(ledState ? "LED is ON" : "LED is OFF");

        // Send the updated LED state to ThingsBoard
        String payload = "{\"ledState\":" + String(ledState) + "}";
        if (client.publish("SSN/Project/IoT102/test/telemetry", payload.c_str())) {
            Serial.println("LED state sent to ThingsBoard");
        } else {
            Serial.println("Failed to send LED state");
        }
    }
}

// Reconnect to MQTT broker
void reconnect() {
    while (!client.connected()) {
        // Reconnect to Wi-Fi if disconnected
        if (WiFi.status() != WL_CONNECTED) {
            WiFiManager wifiManager;
            wifiManager.autoConnect("MBL-IOT");
        }

        // Attempt to connect to MQTT broker
        Serial.print("Connecting to ThingsBoard...");
        if (client.connect("ESP32 Device", TOKEN, NULL)) {
            Serial.println("[DONE]");
            // Subscribe to RPC requests
            client.subscribe("v1/devices/me/rpc/request/+");
        } else {
            Serial.print("[FAILED] rc=");
            Serial.print(client.state());
            Serial.println(" : retrying in 5 seconds");
            delay(5000);
        }
    }
}
