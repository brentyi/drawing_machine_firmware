#include <Arduino.h>
#include "Parser.h"
#include "Configuration.h"

Parser::Parser() {
  buffer_count_ = 0;
}

void Parser::handle(char c) {
  if (c == '\n' || c == ';') {
    buffer_[buffer_count_] = '\0';
    execute_buffer_();
  } else if (buffer_count_ < BUFFER_LENGTH - 1) {
    buffer_[buffer_count_++] = c;
  }
}

void Parser::execute_buffer_() {
  switch (seek_int_('M', -1)) {
    case -1:
      break;
    case 0:
      // unconditional stop
      ready("unconditional stop");
      return;
    case 17:
      // enable steppers
      ready("enabling steppers");
      return;
    case 18:
      // disable steppers
      ready("disabling steppers");
      return;
    case 105:
      // read temperatures
      ready("T:22 B:22");
      return;
    case 112:
      // emergency stop
      ready("emergency stop");
      return;
  }

  switch (seek_int_('G', -1)) {
    case -1:
      break;
    case 0:
      // rapid
      ready("rapid");
      return;
    case 1:
      ready("linear");
      return;
    case 28:
      //home
      ready("homed");
      return;
    case 92:
      //set position
      ready("set");
      return;
  }
  ready("ignoring unsupported gcode");
}

// announce that we're ready for the next command
void Parser::ready(char msg[]) {
  buffer_count_ = 0;
  Serial.print("ok ");
  Serial.println(msg);
}

// find and return the integer that follows some prefix in our buffer
//
// if we have 'G100' buffered and we call seek with prefix = 'G',
// we should return 100
int32_t Parser::seek_int_(char prefix, int default_value) {
  char *ptr = strchr(buffer_, prefix) + 1;
  if (!ptr || ptr < buffer_ || ptr >= buffer_ + buffer_count_) {
    return default_value;
  }
  return atoi(ptr);
}

// floating point version of seek_int_()
float Parser::seek_float_(char prefix, float default_value) {
  char *ptr = strchr(buffer_, prefix) + 1;
  if (!ptr || ptr < buffer_ || ptr >= buffer_ + buffer_count_) {
    return default_value;
  }
  return atof(ptr);
}
