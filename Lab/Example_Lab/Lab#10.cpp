/*
This code connects an ESP32 to ThingsBoard using MQTT and sends temperature and humidity readings from a DHT22 sensor with dynamic Wi-Fi configuration using WiFiManager.
*/

#include <WiFiManager.h> // WiFiManager library for dynamic Wi-Fi configuration
#include <PubSubClient.h>
#include "DHT.h"

// DHT sensor configuration
#define DHT_PIN 4      // Pin connected to the DHT sensor
#define DHT_TYPE DHT11 // DHT sensor type

// ThingsBoard server and access token
<<<<<<< HEAD
const char *thingsboardServer = "your_thingboards_server"; // Replace with your ThingsBoard server
const int mqttPort = 1883;
const char *accessToken = "your_device_access_token"; // Replace with your device access token
=======
const char *thingsboardServer = "159.223.80.40"; // Replace with your ThingsBoard server
const int mqttPort = 1883;
const char *accessToken = "lmi9i60shg21zjtopfew"; // Replace with your device access token
>>>>>>> d3c87615c38a4af34fbcf3b1f0203124bf02cd66

// WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// DHT sensor object
DHT dht(DHT_PIN, DHT_TYPE);

// Timer variables
unsigned long previousMillis = 0;
const unsigned long interval = 5000; // Interval to send data (5 seconds)

// MQTT connection function
void connectMqtt()
{
    client.setServer(thingsboardServer, mqttPort);

    while (!client.connected())
    {
        Serial.println("Connecting to ThingsBoard...");

        if (client.connect("ESP32", accessToken, NULL))
        {
            Serial.println("Connected to ThingsBoard");
        }
        else
        {
            Serial.print("Failed to connect. State: ");
            Serial.println(client.state());
            delay(2000);
        }
    }
}

void setup()
{
    Serial.begin(9600);
    Serial.println("Starting ESP32");

    // Initialize WiFiManager
    WiFiManager wifiManager;

    // Optional: Uncomment the line below to reset Wi-Fi settings (for testing or reconfiguration)
    // wifiManager.resetSettings();

    // Automatically connect or create a configuration portal
    if (!wifiManager.autoConnect("ESP32-ConfigAP"))
    {
        Serial.println("Failed to connect to WiFi, restarting...");
        ESP.restart();
    }

    Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Connect to MQTT server
    connectMqtt();

    // Initialize the DHT sensor
    dht.begin();
}

void loop()
{
    // Reconnect to MQTT broker if disconnected
    if (!client.connected())
    {
        connectMqtt();
    }

    client.loop();

    // Send sensor data at regular intervals
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
        float temperature = dht.readTemperature();
        float humidity = dht.readHumidity();

        if (isnan(temperature) || isnan(humidity))
        {
            Serial.println("Failed to read from DHT sensor!");
            return;
        }

        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print(" °C, Humidity: ");
        Serial.print(humidity);
        Serial.println(" %");

        // Create JSON payload
        String payload = "{";
        payload += "\"temperature\":" + String(temperature) + ",";
        payload += "\"humidity\":" + String(humidity);
        payload += "}";

        // Publish data to ThingsBoard
        if (client.publish("SSN/Project/IoT102/test/telemetry", payload.c_str()))
        {
            Serial.println("Data sent successfully");
        }
        else
        {
            Serial.println("Failed to send data");
        }

        previousMillis = currentMillis;
    }
}
