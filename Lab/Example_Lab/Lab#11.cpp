#include <Arduino.h>
#include <WiFiManager.h>  // WiFiManager library for dynamic Wi-Fi configuration
#include <PubSubClient.h>
#include "DHT.h"

#define TOKEN "lmi9i60shg21zjtopfew"  // ThingsBoard token
#define DHT_PIN 4
#define DHT_TYPE DHT11
#define LED_BUILTIN 5  // Built-in LED pin for ESP32
#define BUTTON_PIN 12  // GPIO pin for the push button

char thingsboardServer[] = "159.223.80.40";

WiFiClient wifiClient;
PubSubClient client(wifiClient);
DHT dht(DHT_PIN, DHT_TYPE);

unsigned long previousMillis = 0;
const unsigned long interval = 5000;  // Interval to send DHT data
bool ledState = LOW;                  // LED state
bool lastButtonState = HIGH;          // Last button state

// Function prototypes
void reconnect();

void setup() {
  Serial.begin(9600);

  // Initialize the built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);  // Ensure LED is off initially

  // Configure the button with an internal pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize WiFiManager
  WiFiManager wifiManager;

  // Optional: Uncomment to reset Wi-Fi settings for testing
  // wifiManager.resetSettings();

  // Automatically connect or create a configuration portal
  if (!wifiManager.autoConnect("ESP32-ConfigAP")) {
    Serial.println("Failed to connect to WiFi, restarting...");
    ESP.restart();
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Set MQTT server and initialize DHT sensor
  client.setServer(thingsboardServer, 1883);
  dht.begin();
}

void loop() {
  // Reconnect to MQTT broker if disconnected
  if (!client.connected()) {
    reconnect();
  }

  // Handle incoming MQTT messages
  client.loop();

  // Read button state and toggle LED
  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
    Serial.println(ledState ? "LED OFF" : "LED ON");
  }
  lastButtonState = currentButtonState;

  // Send DHT sensor data at regular intervals
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Create JSON payload and publish to MQTT
    String payload = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
    if (client.publish("SSN/Project/IoT102/test/telemetry", payload.c_str())) {
      Serial.println("Data sent successfully");
    } else {
      Serial.println("Failed to send data");
    }
    previousMillis = currentMillis;
  }
}

// Reconnect to MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard...");
    if (client.connect("ESP32 Device", TOKEN, NULL)) {
      Serial.println("[DONE]");
    } else {
      Serial.print("[FAILED] rc=");
      Serial.print(client.state());
      Serial.println(" : retrying in 5 seconds");
      delay(5000);
    }
  }
}
