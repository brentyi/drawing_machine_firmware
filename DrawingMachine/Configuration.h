/*
   Drawing machine configuration file
   Standard units: millimeters, radians
*/

/// Physical properties
#define TABLE_DIAMETER 251
#define STEPS_PER_MM 1600 // 200 steps per revolution * 16 microsteps / 2mm per revolution
#define STEPS_PER_RADIAN 4309.42615 // 200 steps per revolution * 16 microsteps * 110 / 13 gear ratio / 2pi

/// Directions
  #define LINEAR_DIR 1
#define ROTARY_DIR 1

/// Movement
#define LINEAR_HOMESPEED 30000
#define LINEAR_MAXSPEED 30000
#define ROTARY_MAXSPEED 2000
#define LINEAR_ACCEL 50000
#define ROTARY_ACCEL 50000

#define SEGMENTATION_LENGTH 1

/// Pin configuration
#define PIN_ROTARY_STEP 2
#define PIN_ROTARY_DIR 3

#define PIN_LINEAR_STEP 5
#define PIN_LINEAR_DIR 4

#define PIN_ENABLE 12

#define PIN_LINEAR_MINSTOP A0

#define PIN_SOLENOID 6

#define PIN_LED_BLUE A1
#define PIN_LED_RED A2

#define PIN_BUTTON A3

/// Firmware-y constants
#define BAUDRATE 115200
#define BUFFER_LENGTH 100

// Default drawing
#define DEFAULT_DRAWING 
