#include "MPU6050.h"

namespace {
  constexpr uint8_t REG_PWR_MGMT_1 = 0x6B;
  constexpr uint8_t REG_WHO_AM_I = 0x75;
  constexpr uint8_t REG_ACCEL_XOUT_H = 0x3B;

  constexpr float ACCEL_SCALE = 16384.0f; // LSB per g at +-2g
  constexpr float GYRO_SCALE = 131.0f;    // LSB per deg/s at +-250 deg/s
}

MPU6050::MPU6050(uint8_t address) : address(address) {}

bool MPU6050::begin() {
  Wire.beginTransmission(address);
  Wire.write(REG_WHO_AM_I);
  if (Wire.endTransmission(false) != 0) return false;

  Wire.requestFrom(address, (uint8_t)1);
  if (Wire.available() != 1) return false;

  // WHO_AM_I reports the device address with bit 0 reserved.
  uint8_t whoAmI = Wire.read();
  if ((whoAmI & 0x7E) != 0x68) return false;

  writeRegister(REG_PWR_MGMT_1, 0x00); // wake up device
  return true;
}

void MPU6050::update() {
  Wire.beginTransmission(address);
  Wire.write(REG_ACCEL_XOUT_H);
  Wire.endTransmission(false);
  Wire.requestFrom(address, (uint8_t)14);

  if (Wire.available() < 14) return;

  int16_t rawAccelX = (Wire.read() << 8) | Wire.read();
  int16_t rawAccelY = (Wire.read() << 8) | Wire.read();
  int16_t rawAccelZ = (Wire.read() << 8) | Wire.read();
  int16_t rawTemp   = (Wire.read() << 8) | Wire.read();
  int16_t rawGyroX  = (Wire.read() << 8) | Wire.read();
  int16_t rawGyroY  = (Wire.read() << 8) | Wire.read();
  int16_t rawGyroZ  = (Wire.read() << 8) | Wire.read();

  accelX = rawAccelX / ACCEL_SCALE;
  accelY = rawAccelY / ACCEL_SCALE;
  accelZ = rawAccelZ / ACCEL_SCALE;

  gyroX = rawGyroX / GYRO_SCALE;
  gyroY = rawGyroY / GYRO_SCALE;
  gyroZ = rawGyroZ / GYRO_SCALE;

  // Per MPU6050 datasheet temperature conversion formula.
  temperature = rawTemp / 340.0f + 36.53f;
}

float MPU6050::getAccelX() const { return accelX; }
float MPU6050::getAccelY() const { return accelY; }
float MPU6050::getAccelZ() const { return accelZ; }

float MPU6050::getGyroX() const { return gyroX; }
float MPU6050::getGyroY() const { return gyroY; }
float MPU6050::getGyroZ() const { return gyroZ; }

float MPU6050::getTemperature() const { return temperature; }

void MPU6050::writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}
