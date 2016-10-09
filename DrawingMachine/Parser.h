#ifndef PARSER_H
#define PARSER_H

// Helper class for G-Code parsing

#include <stdint.h>
#include "Configuration.h"

class Parser {
private:
  char buffer_[BUFFER_LENGTH];
  uint8_t buffer_count_;
  void execute_buffered_();
  float seek_int_(char prefix, int default_value = -1);
  float seek_float_(char prefix, float default_value = -1);
public:
  Parser();
  void handle(char c);
  void ready();
  void ready(char msg[]);
};

#endif //PARSER_H
