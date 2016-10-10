/*
   Drawing machine configuration file
   Standard units: millimeters, radians
*/


/// Physical properties
#define TABLE_RADIUS 200
#define STEPS_PER_MM 1600 // 200 steps per revolution * 16 microsteps / 2mm per step
#define STEPS_PER_RADIAN 4309.42615 // 200 steps per revolution * 16 microsteps * 110 / 13 gear ratio / 2pi

/// Directions
#define INVERT_SOLENOID 0 // Default: pull pin high to lift pen up 
#define LINEAR_DIR 1
#define ROTARY_DIR 1

/// Pin configuration
#define PIN_ROTARY_STEP 2
#define PIN_ROTARY_DIR 3

#define PIN_LINEAR_STEP 5
#define PIN_LINEAR_DIR 4

#define PIN_ENABLE 12

#define PIN_LINEAR_MINSTOP 6

#define PIN_SOLENOID 7

/// Firmware-y constants
#define BAUDRATE 115200
#define BUFFER_LENGTH 100
