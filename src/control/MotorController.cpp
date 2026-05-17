#include "MotorController.h"

MotorController::MotorController(Motor& m, Encoder& e)
  : motor(m), encoder(e), pid(1.0, 0.0, 0.1) {}

float MotorController::getSpeed(float dt) {
  long ticks = encoder.getTicks();
  long delta = ticks - prevTicks;
  prevTicks = ticks;

  // Speed estimate from encoder ticks over elapsed time
  return delta / dt;
}

void MotorController::update(float targetSpeed, float dt) {
  float currentSpeed = getSpeed(dt);
  float output = pid.compute(targetSpeed, currentSpeed, dt);

  // Use PID output to drive the motor toward the requested speed
  motor.setSpeed(output);
}

void MotorController::setOpenLoop(int speed) {
  // Bypass PID and drive the motor directly
  motor.setSpeed(speed);
}
