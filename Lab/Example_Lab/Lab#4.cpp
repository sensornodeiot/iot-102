#include "DHT.h"

// DHT sensor configuration
#define DHT_PIN 4      // Pin connected to the DHT sensor (change accordingly)
#define DHT_TYPE DHT22 // DHT sensor type (now we're using DHT22)

// Create a DHT object
DHT dht(DHT_PIN, DHT_TYPE);

unsigned long previousMillis = 0;    // Timer variables
const unsigned long interval = 2000; // Interval of 2 seconds

void setup()
{
  Serial.begin(115200);
  Serial.println("Lab #4: DHT22 Temperature and Humidity Sensor");

  // Initialize the DHT sensor
  dht.begin();
}

void loop()
{
  // Check if the interval has elapsed
  unsigned long currentMillis = millis();

  // Send sensor value every 2 seconds
  if (currentMillis - previousMillis >= interval)
  {
    // Read temperature and humidity from the DHT sensor
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Check if any reading failed
    if (isnan(temperature) || isnan(humidity))
    {
      Serial.println("Failed to read data from DHT sensor!");
      return;
    }

    // Print the temperature and humidity
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C\t");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Update the previousMillis variable
    previousMillis = currentMillis;
  }
}