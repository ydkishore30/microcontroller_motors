#include "Motor.h"

Motor::Motor(int rpwm, int lpwm, int chR, int chL)
  : pin_rpwm(rpwm), pin_lpwm(lpwm), ch_r(chR), ch_l(chL) {}

void Motor::begin() {
  // Configure two PWM channels for motor control
  ledcSetup(ch_r, 2000, 8);
  ledcSetup(ch_l, 2000, 8);

  // Attach the channels to the motor driver pins
  ledcAttachPin(pin_rpwm, ch_r);
  ledcAttachPin(pin_lpwm, ch_l);
}

void Motor::setSpeed(int speed) {
  speed = constrain(speed, -255, 255);

  if (speed >= 0) {
    // Drive forward: right PWM active, left PWM off
    ledcWrite(ch_r, speed);
    ledcWrite(ch_l, 0);
  } else {
    // Drive backward: left PWM active, right PWM off
    ledcWrite(ch_r, 0);
    ledcWrite(ch_l, -speed);
  }
}
