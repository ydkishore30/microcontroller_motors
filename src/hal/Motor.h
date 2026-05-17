#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

// Motor class encapsulates two PWM outputs for a bidirectional DC motor.
class Motor {
  int pin_rpwm, pin_lpwm;
  int ch_r, ch_l;

public:
  Motor(int rpwm, int lpwm, int chR, int chL);

  // Initialize PWM channels and attach them to motor pins.
  void begin();

  // Set motor speed in the range [-255, 255].
  // Positive values drive forward, negative values drive backward.
  void setSpeed(int speed);
};

#endif
