#pragma once

// Abstraction for a position feedback sensor. MotorController depends
// on this rather than a concrete sensor (e.g. quadrature Encoder), so
// the feedback source can be swapped without changes to the consumer.
class IEncoder {
public:
  virtual ~IEncoder() = default;

  // Read the current accumulated tick count.
  virtual long getTicks() = 0;
};
