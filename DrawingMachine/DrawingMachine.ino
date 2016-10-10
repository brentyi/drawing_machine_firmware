/*
   Firmware for a turntable-based plotter/drawing machine
   Made for UC Berkeley's Prototyping & Fabrication Course
*/

#include "Configuration.h"
#include "Parser.h"

Parser parser;

void setup() {
  Serial.begin(BAUDRATE);
  Serial.println("Drawing Machine");
  Serial.println("DES INV 22 // Fall 2016 Midterm");
  Serial.println("Nandita Iyer & Brent Yi");

  parser.ready();
}

void loop() {
  // put your main code here, to run repeatedly:
  if ( Serial.available() ) {
    char c = Serial.read();
    parser.handle(c);
  }
}
