#include "INA226.h"

namespace {
  constexpr uint8_t REG_CONFIG = 0x00;
  constexpr uint8_t REG_BUS_VOLTAGE = 0x02;
  constexpr uint8_t REG_POWER = 0x03;
  constexpr uint8_t REG_CURRENT = 0x04;
  constexpr uint8_t REG_CALIBRATION = 0x05;

  constexpr float BUS_VOLTAGE_LSB = 0.00125f; // 1.25 mV per bit
  constexpr uint16_t CONFIG_AVG16_CONTINUOUS = 0x4727;
}

INA226::INA226(uint8_t address) : address(address) {}

bool INA226::begin(float shuntResistorOhms, float maxExpectedCurrentA) {
  currentLSB = maxExpectedCurrentA / 32768.0f;
  powerLSB = 25.0f * currentLSB;

  uint16_t calibration = (uint16_t)(0.00512f / (currentLSB * shuntResistorOhms));
  writeRegister(REG_CALIBRATION, calibration);
  writeRegister(REG_CONFIG, CONFIG_AVG16_CONTINUOUS);

  Wire.beginTransmission(address);
  return Wire.endTransmission() == 0;
}

void INA226::update() {
  uint16_t rawBusVoltage = readRegister(REG_BUS_VOLTAGE);
  int16_t rawCurrent = (int16_t)readRegister(REG_CURRENT);
  uint16_t rawPower = readRegister(REG_POWER);

  busVoltage = rawBusVoltage * BUS_VOLTAGE_LSB;
  current = rawCurrent * currentLSB;
  power = rawPower * powerLSB;
}

float INA226::getBusVoltage() const { return busVoltage; }
float INA226::getCurrent() const { return current; }
float INA226::getPower() const { return power; }

void INA226::writeRegister(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write((uint8_t)(value >> 8));
  Wire.write((uint8_t)(value & 0xFF));
  Wire.endTransmission();
}

uint16_t INA226::readRegister(uint8_t reg) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(address, (uint8_t)2);
  if (Wire.available() < 2) return 0;

  uint8_t hi = Wire.read();
  uint8_t lo = Wire.read();
  return (hi << 8) | lo;
}
