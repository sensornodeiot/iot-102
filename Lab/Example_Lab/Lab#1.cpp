/* 
  Welcome to Internet of Things 102 Workshop
  Lab #1: Hello World in IoT programing by blinking built-in LED
  Let's started!
*/

#include <Arduino.h>


// void setup() function is run code once when the Arduino board is powered on or reset.
// It is typically used to initialize variables, pins, libraries, and other resources.
void setup() 
{

  // Initialize the serial communication at 115200 baud rate
  Serial.begin(115200);

  // Set the built-in LED pin as an output
  pinMode(LED_BUILTIN, OUTPUT);
  
  // Print "Hello World" to the serial monitor
  Serial.println("Hello World");
}


// void loop() function is run code repeatedly after the setup() function has finished executing.
// It is the main part of the code where the actual program logic and actions are implemented.
void loop() 
{
  // Turn on the built-in LED
  digitalWrite(LED_BUILTIN, HIGH); 

  // Wait for 1000 milliseconds (1 second)
  delay(1000); 

  // Turn off the built-in LED
  digitalWrite(LED_BUILTIN, LOW); 

  // Wait for 1000 milliseconds (1 second)
  delay(1000); 
}