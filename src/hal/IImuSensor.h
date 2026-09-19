#pragma once

// Abstraction for an IMU sensor. Code that consumes orientation/motion
// data should depend on this interface rather than a concrete sensor
// (e.g. MPU6050), so the implementation can be swapped without changes
// to the consumer.
class IImuSensor {
public:
  virtual ~IImuSensor() = default;

  // Refresh the cached readings from the sensor.
  virtual void update() = 0;

  virtual float getAccelX() const = 0; // g
  virtual float getAccelY() const = 0;
  virtual float getAccelZ() const = 0;

  virtual float getGyroX() const = 0; // deg/s
  virtual float getGyroY() const = 0;
  virtual float getGyroZ() const = 0;

  virtual float getTemperature() const = 0; // deg C

  // Fused orientation in degrees. Only sensors with onboard fusion (e.g.
  // BNO055) provide it; others report 0.
  virtual float getHeading() const { return 0.0f; }
  virtual float getRoll() const { return 0.0f; }
  virtual float getPitch() const { return 0.0f; }
};
