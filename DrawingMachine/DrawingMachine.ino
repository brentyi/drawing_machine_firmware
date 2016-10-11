#include <AccelStepper.h>
#include <MultiStepper.h>

/*
   Firmware for a turntable-based plotter/drawing machine
   Made for UC Berkeley's Prototyping & Fabrication Course
*/

#include "Configuration.h"
#include "Parser.h"
#include "Motion.h"

Motion motion;
Parser parser;

void setup() {
  Serial.begin(BAUDRATE);
  Serial.println("start");
  Serial.println(">> Drawing Machine");
  Serial.println(">> DES INV 22 // Fall 2016 Midterm");
  Serial.println(">> Nandita Iyer & Brent Yi");

  motion.init();
  parser.attachMotionController(&motion);

  parser.ready();
}

void loop() {
  if (Serial.available())
    parser.handle(Serial.read());
}
