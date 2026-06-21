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
