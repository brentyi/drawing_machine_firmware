#ifndef PARSER_H
#define PARSER_H

// Helper class for G-Code parsing

#include <stdint.h>
#include "Configuration.h"
#include "Motion.h"

class Parser {
  private:
    char buffer_[BUFFER_LENGTH];
    uint8_t buffer_count_;
    void execute_buffer_();
    int32_t seek_int_(char prefix, int default_value = -1);
    float seek_float_(char prefix, float default_value = -1);
    Motion *controller_;
  public:
    Parser();
    void attachMotionController(Motion *m);
    void handle(char c);
    void ready(char comment[] = "");
};

#endif //PARSER_H
