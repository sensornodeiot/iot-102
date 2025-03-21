#include <Arduino.h>
#include <WiFiManager.h>  // Include WiFiManager library
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

#define DHT_PIN 4
#define DHT_TYPE DHT11
#define LED_BUILTIN 5  // Built-in LED pin for ESP32

<<<<<<< HEAD
#define TOKEN "your_device_access_token"  // ThingsBoard access token
char thingsboardServer[] = "your_thingboards_server";  // ThingsBoard server
=======
#define TOKEN "lmi9i60shg21zjtopfew"  // ThingsBoard access token
char thingsboardServer[] = "159.223.80.40";  // ThingsBoard server
>>>>>>> d3c87615c38a4af34fbcf3b1f0203124bf02cd66

WiFiClient wifiClient;
PubSubClient client(wifiClient);
DHT dht(DHT_PIN, DHT_TYPE);

unsigned long previousMillis = 0;
const unsigned long interval = 5000;  // Interval to send DHT data

// Function prototypes
void reconnect();
void on_message(const char* topic, byte* payload, unsigned int length);

void setup() {
  Serial.begin(9600);

  // Initialize the built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);  // Ensure LED is off initially

  // Set up WiFiManager for Wi-Fi configuration
  WiFiManager wifiManager;

  // Optional: Uncomment this to reset saved Wi-Fi credentials
  // wifiManager.resetSettings();

  if (!wifiManager.autoConnect("ESP32-ConfigAP")) {
    Serial.println("Failed to connect to WiFi, restarting...");
    ESP.restart();
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize MQTT client and DHT sensor
  client.setServer(thingsboardServer, 1883);
  client.setCallback(on_message);
  dht.begin();
}

void loop() {
  // Reconnect to MQTT if disconnected
  if (!client.connected()) {
    reconnect();
  }

  // Handle incoming MQTT messages
  client.loop();

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

    // Create JSON payload
    String payload = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
    if (client.publish("SSN/Project/IoT102/test/telemetry", payload.c_str())) {
      Serial.println("Data sent successfully");
    } else {
      Serial.println("Failed to send data");
    }
    previousMillis = currentMillis;
  }
}

// Callback for handling RPC messages
void on_message(const char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("]");

  char message[length + 1];
  strncpy(message, (char*)payload, length);
  message[length] = '\0';

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
    return;
  }

  const char* method = doc["method"];
  bool params = doc["params"];

  Serial.print("RPC method: ");
  Serial.println(method);
  Serial.print("Params: ");
  Serial.println(params);

  if (params == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("LED is ON");
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("LED is OFF");
  }
}

// Reconnect to MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard...");
    if (client.connect("ESP32 Device", TOKEN, NULL)) {
      Serial.println("[DONE]");
      client.subscribe("v1/devices/me/rpc/request/+");  // Subscribe to RPC requests
    } else {
      Serial.print("[FAILED] rc=");
      Serial.print(client.state());
      Serial.println(" : retrying in 5 seconds");
      delay(5000);
    }
  }
}
