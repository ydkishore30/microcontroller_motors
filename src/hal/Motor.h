#pragma once
#include <Arduino.h>
#include "IMotor.h"

class Motor : public IMotor {
private:
  int pin_rpwm;
  int pin_lpwm;
  int ch_r;
  int ch_l;

  bool inverted = false;   // ✅ ADD THIS

public:
  Motor(int rpwm, int lpwm, int chR, int chL);

  void begin();
  void setSpeed(float speed) override;

  void setInverted(bool inv);  // ✅ ADD THIS
};