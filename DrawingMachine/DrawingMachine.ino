/*
   Firmware for a turntable-based plotter/drawing machine
   Made for UC Berkeley's Prototyping & Fabrication Course
*/

#include "Configuration.h"
#include "GCodeParser.h"
#include "MotionController.h"

MotionController motion;
GCodeParser parser;

void setup() {
  Serial.begin(BAUDRATE);
  delay(10);
  Serial.println("start");
  Serial.println(">> Drawing Machine");
  Serial.println(">> DES INV 22 // Fall 2016 Midterm");
  Serial.println(">> Nandita Iyer & Brent Yi");

  pinMode(PIN_LED_BLUE, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  for (int i = 0; i < 3; i++) {
    digitalWrite(PIN_LED_RED, HIGH);
    digitalWrite(PIN_LED_BLUE, HIGH);
    delay(500);
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_BLUE, LOW);
    delay(500);
  }
  digitalWrite(PIN_LED_RED, HIGH);

  motion.init();
  parser.attachMotionController(&motion);

  parser.ready();
}

void loop() {
  if (Serial.available()) {
    digitalWrite(PIN_LED_BLUE, HIGH);
    parser.handle(Serial.read());
    digitalWrite(PIN_LED_BLUE, LOW);
  }
}
