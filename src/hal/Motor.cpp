#include "Motor.h"

Motor::Motor(int pwmPin, int dirPin)
  : driver(PWM_DIR, pwmPin, dirPin) {}

void Motor::begin() {
  // CytronMD's constructor already configures both pins - nothing else
  // to set up.
}

void Motor::setSpeed(float speed) {
  speed = constrain(speed, -1.0f, 1.0f);
  if (inverted) speed = -speed;

  driver.setSpeed((int16_t)(speed * 255.0f));
}

void Motor::setInverted(bool inv) {
  inverted = inv;
}
