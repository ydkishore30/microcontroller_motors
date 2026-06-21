#pragma once

// Abstraction for where wheel commands come from. main.cpp depends on
// this rather than a concrete transport (Serial, micro-ROS), so the
// transport can be swapped via build configuration without changing
// the control loop.
class ICommandSource {
public:
  virtual ~ICommandSource() = default;

  // Perform one-time setup for the transport.
  virtual void begin() = 0;

  // Process any pending input. Must be called once per loop iteration.
  virtual void poll() = 0;

  virtual float getLeftCommand() const = 0;
  virtual float getRightCommand() const = 0;
};
