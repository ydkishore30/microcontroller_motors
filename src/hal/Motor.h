#pragma once
#include <Arduino.h>
#include <CytronMotorDriver.h>
#include "IMotor.h"

// Wraps the official CytronMD driver (PWM_DIR mode) for the Cytron MDD3A:
// one PWM (speed) pin + one digital direction pin per motor. Not a
// dual-PWM RPWM/LPWM driver.
class Motor : public IMotor {
private:
  CytronMD driver;
  bool inverted = false;

public:
  Motor(int pwmPin, int dirPin);

  void begin();
  void setSpeed(float speed) override;

  void setInverted(bool inv);
};
