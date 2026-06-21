#include "PID.h"

PID::PID(float p, float i, float d)
  : kp(p), ki(i), kd(d) {}

float PID::compute(float target, float current, float dt) {
  float error = target - current;
  float derivative = (error - prevError) / dt;
  prevError = error;

  float output = kp * error + ki * integral + kd * derivative;

  // Clamp to the motor's valid range and stop integrating once saturated,
  // so the integral term can't wind up while the output is already maxed.
  if (output > 1.0f) {
    output = 1.0f;
  } else if (output < -1.0f) {
    output = -1.0f;
  } else {
    integral += error * dt;
  }

  return output;
}