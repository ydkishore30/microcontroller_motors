#include "MotorController.h"

MotorController::MotorController(IMotor& m, IEncoder& e, IController& c)
  : motor(m), encoder(e), controller(c) {}

float MotorController::getSpeed(float dt) {
  long ticks = encoder.getTicks();
  long delta = ticks - prevTicks;
  prevTicks = ticks;

  // Speed estimate from encoder ticks over elapsed time
  return delta / dt;
}

void MotorController::update(float targetSpeed, float dt) {
  float currentSpeed = getSpeed(dt);
  float output = controller.compute(targetSpeed, currentSpeed, dt);

  // Use PID output to drive the motor toward the requested speed
  motor.setSpeed(output);
}

void MotorController::setOpenLoop(float speed) {
  // Bypass PID and drive the motor directly
  motor.setSpeed(speed);
}

void MotorController::stop() {
  controller.reset();
  motor.setSpeed(0.0f);

  // Resync prevTicks to the current count. getSpeed() only updates
  // prevTicks inside update(), so while stopped (possibly for a long
  // time, across many loop iterations) it goes stale. Without this, the
  // first update() after resuming computes delta against that stale
  // baseline instead of 0, producing a hugely inflated bogus speed and
  // a brief spurious reverse command.
  prevTicks = encoder.getTicks();
}
