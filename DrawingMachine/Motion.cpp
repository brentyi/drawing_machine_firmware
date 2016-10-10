#include <AccelStepper.h>
#include <MultiStepper.h>

#include "Configuration.h"
#include "Motion.h"

void Motion::init() {
  linear_stepper_ = new AccelStepper(AccelStepper::DRIVER, PIN_LINEAR_STEP, PIN_LINEAR_DIR);
  rotary_stepper_ = new AccelStepper(AccelStepper::DRIVER, PIN_ROTARY_STEP, PIN_ROTARY_DIR);

  linear_stepper_->setMaxSpeed(LINEAR_MAXSPEED);
  rotary_stepper_->setMaxSpeed(ROTARY_MAXSPEED);

  linear_stepper_->setAcceleration(LINEAR_ACCEL);
  rotary_stepper_->setAcceleration(ROTARY_ACCEL);

  steppers_ = new MultiStepper();
  steppers_->addStepper(*linear_stepper_);
  steppers_->addStepper(*rotary_stepper_);

  relative_mode_ = false;

  pinMode(PIN_LINEAR_MINSTOP, INPUT_PULLUP);
  pinMode(PIN_ENABLE, OUTPUT);

  disable();
}

void Motion::home() {
  enable();

  linear_stepper_->setSpeed(-1 * LINEAR_HOMESPEED);
  while (digitalRead(PIN_LINEAR_MINSTOP)) {
    linear_stepper_->runSpeed();
  }
  linear_stepper_->move((long) TABLE_RADIUS /  2 * STEPS_PER_MM);
  linear_stepper_->runToPosition();

  linear_stepper_->setCurrentPosition(0);

  rotary_stepper_->move((long) (1.57 * STEPS_PER_RADIAN));
  rotary_stepper_->setCurrentPosition(0);

  setPosition(0, 0, 0);
}

void Motion::disable() {
  digitalWrite(PIN_ENABLE, LOW);
  digitalWrite(PIN_SOLENOID, LOW);
}

void Motion::enable() {
  digitalWrite(PIN_ENABLE, HIGH);
  digitalWrite(PIN_SOLENOID, HIGH);
}

void Motion::relative() {
  relative_mode_ = true;
}

void Motion::absolute() {
  relative_mode_ = false;
}

void Motion::setPosition(float x, float y, float e) {
  position_x_ = x;
  position_y_ = y;
  position_e_ = e;
}

void Motion::move(float x, float y, float e) {
  if(relative_mode_) {
    x += position_x_;
    y += position_y_;
    e += position_e_;
  }

  digitalWrite(PIN_SOLENOID, (e <= position_e_) ^ INVERT_SOLENOID);
  position_e_ = e;

  long positions[2] = {
    sqrt(x * x + y * y) * (long) STEPS_PER_MM, //linear position
    (long) (atan2(y, x) * STEPS_PER_RADIAN) //rotary position
  };
  steppers_->moveTo(positions);
  steppers_->runSpeedToPosition();
}

float Motion::getStationaryX() {
  return relative_mode_ ? 0 : position_x_;
}

float Motion::getStationaryY() {
  return relative_mode_ ? 0 : position_y_;
}

float Motion::getStationaryE() {
  return relative_mode_ ? 0 : position_e_;
}
