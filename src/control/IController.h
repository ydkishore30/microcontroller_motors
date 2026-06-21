#pragma once

// Abstraction for a single-loop feedback controller. MotorController
// depends on this rather than a concrete algorithm (e.g. PID), so the
// control law can be swapped without changes to the consumer.
class IController {
public:
  virtual ~IController() = default;

  // Compute the control output for one step given target/current
  // values and the elapsed time since the previous call.
  virtual float compute(float target, float current, float dt) = 0;
};
