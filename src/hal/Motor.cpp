#include "Motor.h"

Motor::Motor(int rpwm, int lpwm, int chR, int chL)
  : pin_rpwm(rpwm), pin_lpwm(lpwm), ch_r(chR), ch_l(chL) {}

void Motor::begin() {
  ledcSetup(ch_r, 2000, 8);
  ledcSetup(ch_l, 2000, 8);

  ledcAttachPin(pin_rpwm, ch_r);
  ledcAttachPin(pin_lpwm, ch_l);
}

void Motor::setSpeed(int speed) {
  speed = constrain(speed, -255, 255);

  int pwm = abs(speed);

  // STOP condition
  if (speed == 0) {
    ledcWrite(ch_r, 0);
    ledcWrite(ch_l, 0);
    return;
  }

  // FORWARD
  if (speed > 0) {
    ledcWrite(ch_r, pwm);   // RPWM = forward
    ledcWrite(ch_l, 0);
  }

  // BACKWARD
  else {
    ledcWrite(ch_r, 0);
    ledcWrite(ch_l, pwm);   // LPWM = reverse
  }
}