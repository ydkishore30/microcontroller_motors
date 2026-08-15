#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include "../hal/IMotor.h"
#include "../hal/IEncoder.h"
#include "IController.h"

// MotorController performs closed-loop speed control for a given motor and
// encoder pair. It can also be used in open-loop mode when direct PWM control
// is desired. It depends only on abstractions (IMotor/IEncoder/IController)
// so the actuator, feedback sensor and control algorithm can each be swapped
// independently.
class MotorController {
  IMotor& motor;
  IEncoder& encoder;
  IController& controller;

  long prevTicks = 0;

public:
  MotorController(IMotor& m, IEncoder& e, IController& c);

  // Calculate wheel speed from encoder ticks and elapsed time.
  float getSpeed(float dt);

  // Update motor power using PID controller to reach target speed.
  void update(float targetSpeed, float dt);

  // Set raw motor power without feedback control.
  void setOpenLoop(float speed);

  // Clear controller state (e.g. PID integral) and stop the motor
  // immediately, instead of letting leftover correction unwind.
  void stop();
};

#endif
