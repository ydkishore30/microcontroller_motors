#pragma once

// Abstraction for a current/power sensor. Code that consumes power
// telemetry should depend on this interface rather than a concrete
// sensor (e.g. INA226), so the implementation can be swapped without
// changes to the consumer.
class ICurrentSensor {
public:
  virtual ~ICurrentSensor() = default;

  // Refresh the cached readings from the sensor.
  virtual void update() = 0;

  virtual float getBusVoltage() const = 0; // V
  virtual float getCurrent() const = 0;    // A
  virtual float getPower() const = 0;      // W
};
