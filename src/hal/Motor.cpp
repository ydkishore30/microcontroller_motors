#include "Motor.h"

Motor::Motor(int rpwm, int lpwm, int chR, int chL)
  : pin_rpwm(rpwm), pin_lpwm(lpwm), ch_r(chR), ch_l(chL) {}

void Motor::begin() {
  ledcSetup(ch_r, 2000, 8);
  ledcSetup(ch_l, 2000, 8);

  ledcAttachPin(pin_rpwm, ch_r);
  ledcAttachPin(pin_lpwm, ch_l);
}

void Motor::setSpeed(float speed) {

  speed = constrain(speed, -1.0f, 1.0f);

  int pwm = (int)(fabs(speed) * 255.0f);

  if (speed == 0.0f) {
    ledcWrite(ch_r, 0);
    ledcWrite(ch_l, 0);
    return;
  }

  if (speed < 0.0f) {
    ledcWrite(ch_r, pwm);
    ledcWrite(ch_l, 0);
  } else {
    ledcWrite(ch_r, 0);
    ledcWrite(ch_l, pwm);
  }
}