#pragma once
#include <Arduino.h>
#include <CytronMotorDriver.h>
#include "IMotor.h"

// Wraps the official CytronMD driver (PWM_PWM mode) for the Cytron
// MDD3A: the MDD3A has no dedicated direction pin - each motor gets two
// PWM-capable inputs (its datasheet calls them e.g. M1A/M1B), where M1A
// drives forward and M1B drives backward, only one active at a time,
// proportional to speed. Not a PWM+DIR driver.
class Motor : public IMotor {
private:
  CytronMD driver;
  bool inverted = false;

public:
  Motor(int pinA, int pinB);

  void begin();
  void setSpeed(float speed) override;

  void setInverted(bool inv);
};
