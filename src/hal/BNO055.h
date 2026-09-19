#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "IImuSensor.h"

// BNO055 9-axis IMU with onboard sensor fusion, read over I2C. Runs in
// NDOF fusion mode with the default units (accel m/s^2, gyro deg/s), and
// converts accel to g so it matches IImuSensor (and the MPU6050 driver).
class BNO055 : public IImuSensor {
public:
  // 0x28 with the address pin low (default), 0x29 with it tied high.
  static const uint8_t DEFAULT_ADDRESS = 0x28;

  explicit BNO055(uint8_t address = DEFAULT_ADDRESS);

  // Verify the chip ID and switch the sensor into NDOF fusion mode.
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

  float getHeading() const override; // deg, 0-360
  float getRoll() const override;    // deg
  float getPitch() const override;   // deg

private:
  uint8_t address;

  float accelX = 0, accelY = 0, accelZ = 0;
  float gyroX = 0, gyroY = 0, gyroZ = 0;
  float temperature = 0;
  float heading = 0, roll = 0, pitch = 0;

  bool writeRegister(uint8_t reg, uint8_t value);
  bool readRegisters(uint8_t reg, uint8_t* buf, uint8_t len);
};
