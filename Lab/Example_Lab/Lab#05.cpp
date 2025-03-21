#include <WiFi.h>

// Set the name and password of the WiFi network you want to connect to
const char *ssid = "WIFI_NAME";
const char *password = "WIFI_PASSWORD";

// Function to connect to WiFi
void connectWifi()
{

  // Start the WiFi library
  WiFi.begin(ssid, password);

  // Print "Connecting to WiFi.." while the WiFi is connecting
  Serial.print("WiFi: Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // Print the SSID and status of the connected WiFi network
  Serial.println("");
  Serial.print("WiFi SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("WiFi Status: ");
  Serial.println("Connected");
  delay(2000);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Lab #5: WiFi Connectivity");
    connectWifi();
}

void loop()
{
  // Reconnect wifi if the connection is lost
  if (WiFi.status() != WL_CONNECTED)
  {
    connectWifi();
  }
}