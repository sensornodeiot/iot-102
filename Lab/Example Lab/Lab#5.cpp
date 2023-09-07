#include <WiFi.h>

const char *ssid = "WIFI_NAME";
const char *password = "WIFI_PASSWORD";

void connectWifi()
{
    WiFi.begin(ssid, password);
    delay(2000);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.print("Connected to WiFi: ");
    Serial.println(WiFi.SSID());
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Lab #5: WiFi Connectivity");
    connectWifi();
}

void loop()
{
}