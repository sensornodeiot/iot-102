#include <Arduino.h>

unsigned long previousMillis = 0;    // Timer variables
const unsigned long interval = 1000; // Interval of 1 second

bool ledState = LOW;

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  Serial.println("Lab #2: Multi-functional Programming with millis()");
}

void loop() 
{
  // Check if the interval has elapsed
  unsigned long currentMillis = millis();

  // Interval has elapsed, perform tasks
  if (currentMillis - previousMillis >= interval) {   

    // Toggle the LED state
    ledState = !ledState;

    // Write the built-in LED based on ledState variable
    digitalWrite(LED_BUILTIN, ledState);
    
    // Update the previousMillis variable
    previousMillis = currentMillis;
  }
}