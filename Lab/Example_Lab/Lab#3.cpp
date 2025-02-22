#include <Arduino.h>

unsigned long previousMillis = 0;    // Timer variables
const unsigned long interval = 1000; // Interval of 1 second

int counter = 0;

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  Serial.println("Lab #3: Timer and Counter");
}

void loop() 
{
  // Check if the interval has elapsed
  unsigned long currentMillis = millis();

  // Interval has elapsed, perform tasks
  if (currentMillis - previousMillis >= interval) 
  {   
    // Print the current counter value
    Serial.print("Counter: ");
    Serial.println(counter);

    // Increment the counter
    counter++;
    
    // Update the previousMillis variable
    previousMillis = currentMillis;
  }
}