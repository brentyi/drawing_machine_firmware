#ifndef PARSER_H
#define PARSER_H

// Helper class for G-Code parsing

#include <stdint.h>
#include "Configuration.h"
#include "MotionController.h"

class GCodeParser {
  private:
    char buffer_[BUFFER_LENGTH];
    uint8_t buffer_count_;
    MotionController *controller_;

    void executeBuffer_();

    int32_t seekInt_(char prefix, int default_value = -1);
    float seekFloat_(char prefix, float default_value = -1);
  public:
    GCodeParser();
    void attachMotionController(MotionController *m);
    void handle(char c);
    void ready(char comment[] = "");
};

#endif //PARSER_H
