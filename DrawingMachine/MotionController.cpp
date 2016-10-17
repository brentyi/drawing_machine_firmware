#include <AccelStepper.h>
#include <MultiStepper.h>

#include "Configuration.h"
#include "MotionController.h"

void MotionController::init() {
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
  setPenState(PenState::DOWN);
}

void MotionController::home() {
  enable();
  setPenState(PenState::UP);

  linear_stepper_->setSpeed(-1 * LINEAR_HOMESPEED);
  while (digitalRead(PIN_LINEAR_MINSTOP)) {
    linear_stepper_->runSpeed();
  }

  rotary_position_ = 0; 
  linear_position_ = -TABLE_DIAMETER / 2.0;
  setPosition(linear_position_, 0, 0);
  move(position_x_ * 0.95, 0, 0);
}

void MotionController::disable() {
  digitalWrite(PIN_ENABLE, LOW);
  setPenState(PenState::UP);
}

void MotionController::enable() {
  digitalWrite(PIN_ENABLE, HIGH);
}

void MotionController::relative() {
  relative_mode_ = true;
}

void MotionController::absolute() {
  relative_mode_ = false;
}

void MotionController::setPenState(PenState state) {
  if(state == pen_state_) {
    return;
  }
  
  if(state == PenState::UP) {
    analogWrite(PIN_SOLENOID, 255);
    delay(200);
    analogWrite(PIN_SOLENOID, 180);
  } else {
    analogWrite(PIN_SOLENOID, 0);
  }
  pen_state_ = state;
}

void MotionController::setPosition(float x, float y, float e) {
  position_x_ = x;
  position_y_ = y;
  position_e_ = e;
}

void MotionController::moveDirect_(float x, float y, float e) {
  //
  // TODO: probably okay for now, but consider revisiting the implementation of this function
  // - unideal integration of floating point errors over time
  // - no fpu, so all the math is going to take forever
  //
  setPenState(e > position_e_ ? PenState::DOWN : PenState::UP);

  rotary_position_ = fmod(rotary_position_, 2 * PI);

  float linear_goal = -1 * sqrt(x * x + y * y);// * (int32_t) STEPS_PER_MM;
  float rotary_goal = fmod(PI + atan2(y, x), 2 * PI);// * (int32_t) STEPS_PER_RADIAN;

  float linear_delta = linear_goal - linear_position_;
  float rotary_delta = rotary_goal - rotary_position_;

  if (abs(x) < 0.1 && abs(y) < 0.1) {
    //no point in rotating if we're moving to the center
    rotary_delta = 0;
  }
  
  if (rotary_delta > PI) {
    rotary_delta -= 2 * PI;
  } else if (rotary_delta < -PI) {
    rotary_delta += 2 * PI;
  }
  
  int32_t positions[2] = {
    (int32_t) (linear_delta * STEPS_PER_MM),
    (int32_t) (rotary_delta * STEPS_PER_RADIAN)
  };

  linear_stepper_->setCurrentPosition(0);
  rotary_stepper_->setCurrentPosition(0);
  steppers_->moveTo(positions);
  steppers_->runSpeedToPosition();

  linear_position_ = linear_goal;
  rotary_position_ = rotary_goal;
}
void MotionController::move(float x, float y, float e) {
  enable();

  if(relative_mode_) {
    x += position_x_;
    y += position_y_;
    e += position_e_;
  }
  float x_offset = x - position_x_;
  float y_offset = y - position_y_;
  float e_offset = e - position_e_;
  float distance = sqrt(x_offset * x_offset + y_offset * y_offset);

  // partition longer "write" movements and complete them in segments
  // hackish compensation for the nonlinearity of the motion system
  // TODO: properly figure out the math/motion profiling for this

  uint8_t segments = pen_state_ == PenState::DOWN ? distance / SEGMENTATION_LENGTH + 1 : 1;
  Serial.print("Broke linear movement into ");
  Serial.print(segments);
  Serial.println(" segments");

  for(int i = 1; i <= segments; i++){
    Serial.println(i);
    moveDirect_(
      position_x_ + x_offset * i / segments,
      position_y_ + y_offset * i / segments,
      position_e_ + e_offset * i / segments
    );
  }

  position_x_ = x;
  position_y_ = y;
  position_e_ = e;
}

float MotionController::getStationaryX() {
  return relative_mode_ ? 0 : position_x_;
}
float MotionController::getStationaryY() {
  return relative_mode_ ? 0 : position_y_;
}
float MotionController::getStationaryE() {
  return relative_mode_ ? 0 : position_e_;
}

float MotionController::getX() {
  return position_x_;
}
float MotionController::getY() {
  return position_y_;
}
float MotionController::getE() {
  return position_e_;
}

