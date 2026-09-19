#include "BNO055.h"

namespace {
  constexpr uint8_t REG_CHIP_ID = 0x00;
  constexpr uint8_t REG_ACC_DATA_X_LSB = 0x08; // accel, mag, gyro follow contiguously
  constexpr uint8_t REG_EULER_H_LSB = 0x1A; // heading, roll, pitch
  constexpr uint8_t REG_TEMP = 0x34;
  constexpr uint8_t REG_OPR_MODE = 0x3D;
  constexpr uint8_t REG_PWR_MODE = 0x3E;
  constexpr uint8_t REG_PAGE_ID = 0x07;

  constexpr uint8_t CHIP_ID = 0xA0;
  constexpr uint8_t MODE_CONFIG = 0x00;
  constexpr uint8_t MODE_NDOF = 0x0C;

  constexpr float ACCEL_LSB_PER_MS2 = 100.0f; // default unit: 1 LSB = 0.01 m/s^2
  constexpr float GRAVITY = 9.80665f;
  constexpr float GYRO_LSB_PER_DPS = 16.0f;   // default unit: 1 LSB = 1/16 deg/s
  constexpr float EULER_LSB_PER_DEG = 16.0f;  // default unit: 1 LSB = 1/16 deg
}

BNO055::BNO055(uint8_t address) : address(address) {}

bool BNO055::begin() {
  // The chip needs ~650ms after power-up before it answers, so retry the
  // ID read for a while instead of failing on the first attempt.
  uint8_t id = 0;
  bool found = false;
  for (int i = 0; i < 10 && !found; i++) {
    found = readRegisters(REG_CHIP_ID, &id, 1) && id == CHIP_ID;
    if (!found) delay(100);
  }
  if (!found) return false;

  if (!writeRegister(REG_OPR_MODE, MODE_CONFIG)) return false;
  delay(25);
  writeRegister(REG_PAGE_ID, 0x00);
  writeRegister(REG_PWR_MODE, 0x00); // normal power
  if (!writeRegister(REG_OPR_MODE, MODE_NDOF)) return false;
  delay(25);

  uint8_t mode = 0xFF;
  readRegisters(REG_OPR_MODE, &mode, 1);

  Serial.print("IMU begin OK: BNO055 chipId=0x");
  Serial.print(id, HEX);
  Serial.print(" oprMode=0x");
  Serial.println(mode, HEX);

  return mode == MODE_NDOF;
}

void BNO055::update() {
  // One burst: accel (6 bytes), mag (6, unused), gyro (6). Reading them
  // together keeps each 16-bit value from tearing between its two bytes.
  uint8_t buf[18];
  if (!readRegisters(REG_ACC_DATA_X_LSB, buf, sizeof(buf))) return;

  auto s16 = [&](int i) { return (int16_t)(buf[i] | (buf[i + 1] << 8)); };

  accelX = s16(0) / ACCEL_LSB_PER_MS2 / GRAVITY;
  accelY = s16(2) / ACCEL_LSB_PER_MS2 / GRAVITY;
  accelZ = s16(4) / ACCEL_LSB_PER_MS2 / GRAVITY;

  gyroX = s16(12) / GYRO_LSB_PER_DPS;
  gyroY = s16(14) / GYRO_LSB_PER_DPS;
  gyroZ = s16(16) / GYRO_LSB_PER_DPS;

  uint8_t euler[6];
  if (readRegisters(REG_EULER_H_LSB, euler, sizeof(euler))) {
    heading = (int16_t)(euler[0] | (euler[1] << 8)) / EULER_LSB_PER_DEG;
    roll = (int16_t)(euler[2] | (euler[3] << 8)) / EULER_LSB_PER_DEG;
    pitch = (int16_t)(euler[4] | (euler[5] << 8)) / EULER_LSB_PER_DEG;
  }

  uint8_t temp;
  if (readRegisters(REG_TEMP, &temp, 1)) temperature = (int8_t)temp; // 1 LSB = 1 deg C
}

float BNO055::getAccelX() const { return accelX; }
float BNO055::getAccelY() const { return accelY; }
float BNO055::getAccelZ() const { return accelZ; }

float BNO055::getGyroX() const { return gyroX; }
float BNO055::getGyroY() const { return gyroY; }
float BNO055::getGyroZ() const { return gyroZ; }

float BNO055::getTemperature() const { return temperature; }

float BNO055::getHeading() const { return heading; }
float BNO055::getRoll() const { return roll; }
float BNO055::getPitch() const { return pitch; }

bool BNO055::writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

bool BNO055::readRegisters(uint8_t reg, uint8_t* buf, uint8_t len) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;

  if (Wire.requestFrom(address, len) != len) return false;
  for (uint8_t i = 0; i < len; i++) buf[i] = Wire.read();
  return true;
}
