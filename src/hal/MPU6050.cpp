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

  if (!writeRegister(REG_PWR_MGMT_1, 0x00)) return false; // wake up device

  // MPU6050 needs time to stabilize after leaving sleep mode before
  // ACCEL/GYRO registers report real samples instead of stale zeros.
  delay(50);

  // Verify the wake-up write actually stuck, not just that it was ACKed.
  Wire.beginTransmission(address);
  Wire.write(REG_PWR_MGMT_1);
  Wire.endTransmission(false);
  Wire.requestFrom(address, (uint8_t)1);
  uint8_t pwrMgmt1 = Wire.available() ? Wire.read() : 0xFF;

  Serial.print("IMU begin OK: whoAmI=0x");
  Serial.print(whoAmI, HEX);
  Serial.print(" pwrMgmt1=0x");
  Serial.println(pwrMgmt1, HEX);

  return true;
}

void MPU6050::update() {
  Wire.beginTransmission(address);
  Wire.write(REG_ACCEL_XOUT_H);
  uint8_t txStatus = Wire.endTransmission(false);
  uint8_t received = Wire.requestFrom(address, (uint8_t)14);

  bool ok = (txStatus == 0 && Wire.available() >= 14);

  static unsigned long lastLog = 0;
  if (millis() - lastLog > 1000) {
    lastLog = millis();
    Serial.print("IMU status: ");
    Serial.print(ok ? "OK" : "FAIL");
    Serial.print(" txStatus=");
    Serial.print(txStatus);
    Serial.print(" requested=14 received=");
    Serial.print(received);
    Serial.print(" available=");
    Serial.println(Wire.available());
  }

  if (!ok) return;

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

bool MPU6050::writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}
