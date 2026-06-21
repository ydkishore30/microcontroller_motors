#pragma once

// Abstraction for a motor actuator. MotorController depends on this
// rather than a concrete driver (e.g. Motor), so the hardware backend
// can be swapped without changes to the consumer.
class IMotor {
public:
  virtual ~IMotor() = default;

  // Drive the motor at the given normalized speed in [-1.0, 1.0].
  virtual void setSpeed(float speed) = 0;
};
