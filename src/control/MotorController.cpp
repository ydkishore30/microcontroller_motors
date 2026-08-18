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

  // Never let the wheel visibly reverse against a nonzero commanded
  // direction. Early in a ramp (tiny target, low/noisy tick counts) the
  // PID math can transiently compute a wrong-sign output - bounded and
  // self-correcting internally, but still a real, undesirable physical
  // jerk on the robot. Clamping here removes that jerk at the one place
  // it actually matters (what reaches the motor) without touching the
  // PID's own internal state/math.
  if (targetSpeed > 0.0f && output < 0.0f) {
    output = 0.0f;
  } else if (targetSpeed < 0.0f && output > 0.0f) {
    output = 0.0f;
  }

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
