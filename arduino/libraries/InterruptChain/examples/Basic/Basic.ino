/**
 * Demo of a chained interrupt handler.
 *
 * This sketch duplicates the signal on digital pin 2 to digital pin 13 (the LED) and
 * echos the value on the serial line. (so open up the serial console @ 9600 baud)
 *
 * Hardware setup for this example:
 *  - Attach a switch from ground to pin 2, or use a wire from ground to pin 2.
 */

#include <InterruptChain.h>

void echoLed() {     
  digitalWrite(13, digitalRead(2));
}

void echoSerial() {
  if (digitalRead(2)) {
    Serial.println("The signal is HIGH!");
  } else {
    Serial.println("The signal is LOW!");
  }
}

void setup() {
    Serial.begin(9600);
    pinMode(13, OUTPUT);

    // Enable internal pull-up on pin 2.
	// This is merely for this demo; it is not required for interrupt handling.
    digitalWrite(2, HIGH);

    // Links two callback to interrupt 0 with default mode CHANGE.    
    InterruptChain::addInterruptCallback(0, echoLed);
    InterruptChain::addInterruptCallback(0, echoSerial);
}

void loop() {
   // You can do other stuff here!
}
