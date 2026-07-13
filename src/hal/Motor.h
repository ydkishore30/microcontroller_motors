#pragma once
#include <Arduino.h>
#include "IMotor.h"

// Driver interface: one PWM (speed) pin + one digital direction pin per
// motor, matching the SmartElex 15D's Independent PWM Mode (DIP switch
// 0011). Not a dual-PWM RPWM/LPWM driver.
class Motor : public IMotor {
private:
  int pin_pwm;
  int pin_dir;
  int ch_pwm;

  bool inverted = false;

public:
  Motor(int pwmPin, int dirPin, int pwmChannel);

  void begin();
  void setSpeed(float speed) override;

  void setInverted(bool inv);
};
