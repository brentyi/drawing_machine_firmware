#include <Arduino.h>
#include "Parser.h"
#include "Configuration.h"

Parser::Parser() {
  buffer_count_ = 0;
}

void Parser::handle(char c) {
  if(buffer_count_ < BUFFER_LENGTH)
    buffer_[buffer_count_++] = c;

  if(c == '\n') {
    buffer_[buffer_count_] = '\0';
    execute_buffered_();
  }
}

float Parser::seek_int_(char prefix, int default_value) {
  char *ptr = buffer_;
  while(ptr && *ptr && ptr < buffer_ + buffer_count_) {
    if(*ptr == prefix) {
      return atoi(ptr + 1);
    }
    ptr = strchr(ptr,' ') + 1;
  }
  return default_value;
}
float Parser::seek_float_(char prefix, float default_value) {
  char *ptr = buffer_;
  while(ptr && *ptr && ptr < buffer_ + buffer_count_) {
    if(*ptr == prefix) {
      return atof(ptr + 1);
    }
    ptr = strchr(ptr,' ') + 1;
  }
  return default_value;
}

void Parser::execute_buffered_() {
  switch((int8_t) seek_int_('G', -1)) {
    case -1:
      break;
    case 0:
      // rapid
      ready();
      return;
    case 1:
      ready();
      return;
    case 28:
      //home
      ready("homed");
      return;
  }
  ready("ignoring unrecognized gcode");
}

void Parser::ready() {
  buffer_count_ = 0;
  Serial.println("ok;");
}
void Parser::ready(char msg[]) {
  buffer_count_ = 0;
  Serial.print("ok; ");
  Serial.println(msg);
}

