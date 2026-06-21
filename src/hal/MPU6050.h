#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "IImuSensor.h"

// MPU6050 reads accelerometer, gyroscope and temperature data over I2C
// at the sensor's default +-2g / +-250 deg/s sensitivity ranges.
class MPU6050 : public IImuSensor {
public:
  static const uint8_t DEFAULT_ADDRESS = 0x68;

  explicit MPU6050(uint8_t address = DEFAULT_ADDRESS);

  // Wake the device and verify it responds on the bus.
  bool begin();

  // Read the latest accel/gyro/temperature values from the sensor.
  void update() override;

  float getAccelX() const override; // g
  float getAccelY() const override;
  float getAccelZ() const override;

  float getGyroX() const override; // deg/s
  float getGyroY() const override;
  float getGyroZ() const override;

  float getTemperature() const override; // deg C

private:
  uint8_t address;

  float accelX = 0, accelY = 0, accelZ = 0;
  float gyroX = 0, gyroY = 0, gyroZ = 0;
  float temperature = 0;

  void writeRegister(uint8_t reg, uint8_t value);
};
