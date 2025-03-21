#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <WiFiManager.h>  // Include WiFiManager library

// ThingsBoard credentials
<<<<<<< HEAD
#define TOKEN "your_device_access_token"
char thingsboardServer[] = "your_thingsboard_server";
=======
#define TOKEN "lmi9i60shg21zjtopfew"
char thingsboardServer[] = "159.223.80.40";
>>>>>>> d3c87615c38a4af34fbcf3b1f0203124bf02cd66

// Pin definitions for DHT sensor and LED
#define DHT_PIN 4
#define DHT_TYPE DHT11
#define LED_PIN 2  // Change to GPIO2 for external LED
#define BUTTON_PIN 12  // Push button pin

// Seven segment display pins
#define SEG_A 13
#define SEG_B 14
#define SEG_C 27
#define SEG_D 26
#define SEG_E 25
#define SEG_F 33
#define SEG_G 32

// LED state representation on seven-segment display
const byte led_on[7] = {0, 1, 1, 0, 0, 0, 0};  // Example for "LED ON"
const byte led_off[7] = {1, 1, 1, 1, 1, 1, 0}; // Example for "LED OFF"

// Global variables
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
void displayNumber(const byte seg[7]);

void setup() {
  Serial.begin(9600);

  // Initialize LED pin and button pin
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize seven-segment display pins
  pinMode(SEG_A, OUTPUT);
  pinMode(SEG_B, OUTPUT);
  pinMode(SEG_C, OUTPUT);
  pinMode(SEG_D, OUTPUT);
  pinMode(SEG_E, OUTPUT);
  pinMode(SEG_F, OUTPUT);
  pinMode(SEG_G, OUTPUT);

  // Initialize WiFiManager
  WiFiManager wifiManager;
  
  // Uncomment the following line for testing auto configuration reset
  // wifiManager.resetSettings();
  
  // Start WiFiManager and configure Wi-Fi connection
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
  // Reconnect to MQTT if disconnected
  if (!client.connected()) {
    reconnect();
  }

  // Handle MQTT messages
  client.loop();

  // Read the push button state
  currentButtonState = digitalRead(BUTTON_PIN);

  // If the button is pressed (LOW state, due to INPUT_PULLUP mode)
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    // Toggle the LED state
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);  // Control LED on GPIO2
    Serial.println(ledState ? "LED is ON" : "LED is OFF");

    // Send the updated LED state to ThingsBoard
    String payload = "{\"ledState\":" + String(ledState) + "}";
    if (client.publish("SSN/Project/IoT102/test/telemetry", payload.c_str())) {
      Serial.println("LED state sent to ThingsBoard");
    } else {
      Serial.println("Failed to send LED state");
    }

    // Update the seven-segment display based on LED state
    if (ledState) {
      displayNumber(led_on);  // Show "LED ON" representation
    } else {
      displayNumber(led_off); // Show "LED OFF" representation
    }
  }

  // Update the lastButtonState variable
  lastButtonState = currentButtonState;

  // Send DHT sensor data at regular intervals
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Read temperature and humidity
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Check if sensor readings are valid
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Print sensor data to Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Create JSON payload
    String payload = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + ",\"ledState\":" + String(ledState) + "}";

    // Publish payload to custom MQTT topic
    if (client.publish("SSN/Project/IoT102/test/telemetry", payload.c_str())) {
      Serial.println("Data sent successfully");
    } else {
      Serial.println("Failed to send data");
    }

    // Update the previousMillis variable
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

  // Print method and params for debugging
  Serial.print("RPC method: ");
  Serial.println(method);
  Serial.print("Params: ");
  Serial.println(params);

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

    // Update the seven-segment display based on LED state
    if (ledState) {
      displayNumber(led_on);  // Show "LED ON" representation
    } else {
      displayNumber(led_off); // Show "LED OFF" representation
    }
  }
//   // Read the push button state
// currentButtonState = digitalRead(BUTTON_PIN);

// // If the button is pressed (LOW state, due to INPUT_PULLUP mode)
// if (currentButtonState == LOW && lastButtonState == HIGH) {
//     // Toggle the LED state
//     ledState = !ledState;
//     digitalWrite(LED_PIN, ledState ? HIGH : LOW);  // Control LED on GPIO2
//     Serial.println(ledState ? "LED is ON" : "LED is OFF");

//     // Send the updated LED state to ThingsBoard
//     String payload = "{\"ledState\":" + String(ledState) + ",\"method\":\"setValue\"}";
//     if (client.publish("SSN/Project/IoT102/test/telemetry", payload.c_str())) {
//         Serial.println("LED state sent to ThingsBoard");
//     } else {
//         Serial.println("Failed to send LED state");
//     }

//     // Update the seven-segment display based on LED state
//     if (ledState) {
//         displayNumber(led_on);  // Show "LED ON" representation
//     } else {
//         displayNumber(led_off); // Show "LED OFF" representation
//     }
// }

// // Update the lastButtonState variable
// lastButtonState = currentButtonState;
}

// Reconnect to MQTT broker
void reconnect() {
  while (!client.connected()) {
    // Reconnect to Wi-Fi if disconnected
    if (WiFi.status() != WL_CONNECTED) {
      WiFiManager wifiManager;
<<<<<<< HEAD
      wifiManager.autoConnect("ESP32-ConfigAP");
=======
      wifiManager.autoConnect("MBL-IOT");
>>>>>>> d3c87615c38a4af34fbcf3b1f0203124bf02cd66
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

// Function to display a number on the seven-segment display
void displayNumber(const byte seg[7]) {
  digitalWrite(SEG_A, seg[0]);
  digitalWrite(SEG_B, seg[1]);
  digitalWrite(SEG_C, seg[2]);
  digitalWrite(SEG_D, seg[3]);
  digitalWrite(SEG_E, seg[4]);
  digitalWrite(SEG_F, seg[5]);
  digitalWrite(SEG_G, seg[6]);
}
