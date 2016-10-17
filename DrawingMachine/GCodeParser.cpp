#include <Arduino.h>
#include "GCodeParser.h"
#include "Configuration.h"

GCodeParser::GCodeParser() {
  buffer_count_ = 0;
}

void GCodeParser::handle(char c) {
  if (c == '\n' || c == ';') {
    buffer_[buffer_count_] = '\0';
    executeBuffer_();
  } else if (buffer_count_ < BUFFER_LENGTH - 1) {
    buffer_[buffer_count_++] = c;
  }
}

void GCodeParser::attachMotionController(MotionController *m) {
  controller_ = m;
}

void GCodeParser::executeBuffer_() {
  switch (seekInt_('M', -1)) {
    case -1:
      break;
    case 0:
      // unconditional stop
      controller_->disable();
      for(;;){
        Serial.println("unconditional stop, please power cycle");
        delay(5000);
      }
      return;
    case 17:
      controller_->enable();
      ready("enabled steppers");
      return;
    case 18:
      controller_->disable();
      ready("disabled steppers");
      return;
    case 84:
      controller_->disable();
      ready("stopped idle hold");
      return;
    case 105:
      // read temperatures
      ready("T:22 B:22");
      return;
    case 112:
      // emergency stop
      controller_->disable();
      for(;;){
        Serial.println("emergency stop, please power cycle");
        delay(5000);
      }
      return;
  }

  switch (seekInt_('G', -1)) {
    case -1:
      break;
    case 0:
      controller_->move(
        seekFloat_('X', controller_->getStationaryX()),
        seekFloat_('Y', controller_->getStationaryY()),
        seekFloat_('E', controller_->getStationaryE())
      );
      ready("rapid");
      return;
    case 1:
      controller_->move(
        seekFloat_('X', controller_->getStationaryX()),
        seekFloat_('Y', controller_->getStationaryY()),
        seekFloat_('E', controller_->getStationaryE())
      );
      ready("linear");
      return;
    case 4:
      delay(seekFloat_('P', 0));
      delay(seekFloat_('S', 0) * 1000);
      ready("dwell");
      return;
    case 28:
      //home
      controller_->home();
      ready("homed");
      return;
    case 90:
      controller_->absolute();
      ready("set absolute mode");
      return;
    case 91:
      controller_->relative();
       ready("set relative mode");
      return;
    case 92:
      //set position
      controller_->setPosition(
        seekFloat_('X', controller_->getX()),
        seekFloat_('Y', controller_->getY()),
        seekFloat_('E', controller_->getE())
      );
      ready("set positions");
      return;
  }
  ready("ignoring unsupported gcode");
}

// announce that we're ready for the next command
void GCodeParser::ready(const char comment[]) {
  buffer_count_ = 0;
  Serial.print("ok ");
  Serial.println(comment);
}

// find and return the integer that follows some prefix in our buffer
//
// if we have 'G100' buffered and we call seek with prefix = 'G',
// this function should return 100
int32_t GCodeParser::seekInt_(char prefix, int default_value) {
  char *ptr = strchr(buffer_, prefix) + 1;
  if (!ptr || ptr < buffer_ || ptr >= buffer_ + buffer_count_) {
    return default_value;
  }
  return atoi(ptr);
}

// floating point version of seekInt_()
float GCodeParser::seekFloat_(char prefix, float default_value) {
  char *ptr = strchr(buffer_, prefix) + 1;
  if (!ptr || ptr < buffer_ || ptr >= buffer_ + buffer_count_) {
    return default_value;
  }
  return atof(ptr);
}
