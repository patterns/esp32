/*
  Blink

  Turn LED on for one second,
  then off for one second,
  repeat.
*/

#include "Arduino.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

void setup() {
  // initialize LED digital pin for output
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // turn LED on
  digitalWrite(LED_BUILTIN, HIGH);

  // wait one second
  delay(1000);

  // turn LED off
  digitalWrite(LED_BUILTIN, LOW);

  // wait one second
  delay(1000);
}

