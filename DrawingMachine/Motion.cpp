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
  pen_state_ = 0;
  setPenState(1);
}

void Motion::home() {
  enable();
  setPenState(true);

  linear_stepper_->setSpeed(-1 * LINEAR_HOMESPEED);
  while (digitalRead(PIN_LINEAR_MINSTOP)) {
    linear_stepper_->runSpeed();
  }
  linear_stepper_->move((long) TABLE_DIAMETER /  2 * STEPS_PER_MM);
  linear_stepper_->runToPosition();

  linear_stepper_->setCurrentPosition(0);

  //rotary_stepper_->move((long) (1.57 * STEPS_PER_RADIAN));
  //rotary_stepper_->runToPosition();
  rotary_stepper_->setCurrentPosition(0);

  setPosition(0, 0, 0);
}

void Motion::disable() {
  digitalWrite(PIN_ENABLE, LOW);
  setPenState(false);
}

void Motion::enable() {
  digitalWrite(PIN_ENABLE, HIGH);
  //setPenState(true);
}

void Motion::relative() {
  relative_mode_ = true;
}

void Motion::absolute() {
  relative_mode_ = false;
}

void Motion::setPenState(bool state) {
  if(state == pen_state_) {
    return;
  }
  
  if(state) {
    analogWrite(PIN_SOLENOID, 255);
    delay(200);
    analogWrite(PIN_SOLENOID, 190);
  } else {
    analogWrite(PIN_SOLENOID, 0);
  }
  pen_state_ = state;
}

void Motion::setPosition(float x, float y, float e) {
  position_x_ = x;
  position_y_ = y;
  position_e_ = e;
}

void Motion::moveDirect(float x, float y, float e) {
  setPenState(e <= position_e_);
  
  position_e_ = e;

  long linear_position = sqrt(x * x + y * y) * (long) STEPS_PER_MM;
  long rotary_position = (long) (atan2(y, x) * STEPS_PER_RADIAN);
  long positions[2] = {
    linear_position,//linear position
    rotary_position //rotary position
  };
  steppers_->moveTo(positions);
  steppers_->runSpeedToPosition();

  position_x_ = x;
  position_y_ = y;
  position_e_ = e;
}
void Motion::move(float x, float y, float e) {
  enable();

  if(relative_mode_) {
    x += position_x_;
    y += position_y_;
    e += position_e_;
  }

  //brent method "This is not the proper way to do it, we need to figure this out"
  float distance = sqrt(x * x + y * y);
  if(distance <= MAX_DIST){
    moveDirect(x,y,e);
  }
  else{
    int frac = distance / MOVE_AMOUNT; 

    float x_offset = x - position_x_;
    float y_offset = y - position_y_;
    float e_offset = e - position_e_;
    for(int i = 1; i <= frac; i++){ //brent you should fix it
      Serial.print(i);
      moveDirect(
        position_x_ + x_offset * i / frac,
        position_y_ + y_offset * i / frac,
        position_e_ + e_offset * i / frac
      );
      Serial.print(frac);
    }
  }
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

float Motion::getX() {
  return position_x_;
}
float Motion::getY() {
  return position_y_;
}
float Motion::getE() {
  return position_e_;
}

