#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "ICurrentSensor.h"

// INA226 measures bus voltage, shunt current and power over I2C.
// begin() must be called with the shunt resistor value and the maximum
// expected current for the application to configure the calibration
// register before readings are valid.
class INA226 : public ICurrentSensor {
public:
  static const uint8_t DEFAULT_ADDRESS = 0x40;

  explicit INA226(uint8_t address = DEFAULT_ADDRESS);

  bool begin(float shuntResistorOhms, float maxExpectedCurrentA);

  // Read the latest bus voltage, current and power values.
  void update() override;

  float getBusVoltage() const override; // V
  float getCurrent() const override;    // A
  float getPower() const override;      // W

private:
  uint8_t address;
  float currentLSB = 0;
  float powerLSB = 0;

  float busVoltage = 0;
  float current = 0;
  float power = 0;

  void writeRegister(uint8_t reg, uint16_t value);
  uint16_t readRegister(uint8_t reg);
};
