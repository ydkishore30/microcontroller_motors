#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include "../hal/Motor.h"
#include "../hal/Encoder.h"
#include "PID.h"

// MotorController performs closed-loop speed control for a given motor and
// encoder pair. It can also be used in open-loop mode when direct PWM control
// is desired.
class MotorController {
  Motor& motor;
  Encoder& encoder;
  PID pid;

  long prevTicks = 0;

public:
  MotorController(Motor& m, Encoder& e);

  // Calculate wheel speed from encoder ticks and elapsed time.
  float getSpeed(float dt);

  // Update motor power using PID controller to reach target speed.
  void update(float targetSpeed, float dt);

  // Set raw motor power without feedback control.
  void setOpenLoop(int speed);
};

#endif
