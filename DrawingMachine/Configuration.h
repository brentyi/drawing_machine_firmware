/*
   Drawing machine configuration file
   Standard units: millimeters, radians
*/

/// Physical properties
#define TABLE_DIAMETER 238
#define STEPS_PER_MM 1600 // 200 steps per revolution * 16 microsteps / 2mm per revolution
#define STEPS_PER_RADIAN 4309.42615 // 200 steps per revolution * 16 microsteps * 110 / 13 gear ratio / 2pi

/// Directions
  #define LINEAR_DIR 1
#define ROTARY_DIR 1

/// Movement related thingys
#define LINEAR_HOMESPEED 30000
#define LINEAR_MAXSPEED 50000
#define ROTARY_MAXSPEED 1000
#define LINEAR_ACCEL 50000
#define ROTARY_ACCEL 5000
#define MAX_DIST 400
#define MOVE_AMOUNT 2

/// Pin configuration
#define PIN_ROTARY_STEP 2
#define PIN_ROTARY_DIR 3

#define PIN_LINEAR_STEP 5
#define PIN_LINEAR_DIR 4

#define PIN_ENABLE 12

#define PIN_LINEAR_MINSTOP A0

#define PIN_SOLENOID 6

/// Firmware-y constants
#define BAUDRATE 115200
#define BUFFER_LENGTH 100
