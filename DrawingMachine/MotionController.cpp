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

  linear_position_ = 0;
  rotary_position_ = 0;

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

  setPosition(-TABLE_DIAMETER / 2.0, 0, 0);
  linear_position_ = position_x_ * STEPS_PER_MM;
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


void MotionController::move(float x, float y, float e) {
  enable();
  setPenState(e > position_e_ ? PenState::DOWN : PenState::UP);

  if(relative_mode_) {
    x += position_x_;
    y += position_y_;
    e += position_e_;
  }
  float x_offset = x - position_x_;
  float y_offset = y - position_y_;
  float distance = sqrt(x_offset * x_offset + y_offset * y_offset);

  // partition longer "write" movements and complete them in segments
  // this sacrifices quality for both physical & computational speed
  // moveStraight_() will make more mathmetically precise movements

  uint8_t segments = pen_state_ == PenState::DOWN ? distance / SEGMENTATION_LENGTH + 1 : 1;
  Serial.print("Broke linear movement into ");
  Serial.print(segments);
  Serial.println(" segments");

  for(int i = 1; i <= segments; i++){
    Serial.println(i);
    moveDirect_(
      position_x_ + x_offset * i / segments,
      position_y_ + y_offset * i / segments
    );
  }

  position_x_ = x;
  position_y_ = y;
  position_e_ = e;
}

void MotionController::moveDirect_(float x, float y) {
  rotary_position_ %= (int32_t) (2 * PI * STEPS_PER_RADIAN);

  int32_t linear_goal = -1 * sqrt(x * x + y * y) * (int32_t) STEPS_PER_MM;
  int32_t rotary_goal = fmod(PI + atan2(y, x), 2 * PI) * (int32_t) STEPS_PER_RADIAN;

  int32_t linear_delta = linear_goal - linear_position_;
  int32_t rotary_delta = rotary_goal - rotary_position_;

  if (abs(x) < 0.1 && abs(y) < 0.1) {
    //no point in rotating if we're moving to the center
    rotary_delta = 0;
  }
  
  if (rotary_delta > PI * STEPS_PER_RADIAN) {
    rotary_delta -= 2 * PI * STEPS_PER_RADIAN;
  } else if (rotary_delta < -PI * STEPS_PER_RADIAN) {
    rotary_delta += 2 * PI * STEPS_PER_RADIAN;
  }
  
  int32_t positions[2] = {
    linear_delta,
    rotary_delta
  };

  linear_stepper_->setCurrentPosition(0);
  rotary_stepper_->setCurrentPosition(0);
  steppers_->moveTo(positions);
  steppers_->runSpeedToPosition();

  linear_position_ = linear_goal;
  rotary_position_ = rotary_goal;
}

void MotionController::moveStraight_(float x, float y) {
  // calculations for reference: http://i.imgur.com/rBGpZ9P.jpg
  
  enable();

  rotary_position_ %= (int32_t) (2 * PI * STEPS_PER_RADIAN);

  float dx = x - position_x_;
  float dy = y - position_y_;
  float distance = sqrt(dx * dx + dy * dy);

  linear_stepper_->setCurrentPosition(0);
  rotary_stepper_->setCurrentPosition(0);

  float theta = -1;
  float p = -1;
  float cosine = -1;
  float sine = -1;
  while (distance > 0.1){ 
    if (p >= 0) {
      linear_stepper_->runSpeed();
      rotary_stepper_->runSpeed();
      linear_position_ += linear_stepper_->currentPosition();
      rotary_position_ += rotary_stepper_->currentPosition();
      linear_stepper_->setCurrentPosition(0);
      rotary_stepper_->setCurrentPosition(0);
    }
    
    theta = rotary_position_ / (float)STEPS_PER_RADIAN;
    p = linear_position_ / (float)STEPS_PER_MM;
    cosine = cos(theta); // avoid recalculating
    sine = sin(theta);
    
    position_x_ = p * cosine;
    position_y_ = p * sine;
  
    dx = position_x_ - x;
    dy = position_y_ - y;
    distance = sqrt(dx * dx + dy * dy);
    dx /= distance; // normalize
    dy /= distance;

    linear_stepper_->setSpeed((dx * cosine + dy * sine) * STEPS_PER_MM * 5);
    rotary_stepper_->setSpeed((dy * cosine / p - dx * sine / p) * STEPS_PER_RADIAN * 5);
  }
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

