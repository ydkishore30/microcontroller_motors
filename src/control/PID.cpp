#include "PID.h"

PID::PID(float p, float i, float d)
  : kp(p), ki(i), kd(d) {}

float PID::compute(float target, float current, float dt) {
  float error = target - current;
  integral += error * dt;
  float derivative = (error - prevError) / dt;
  prevError = error;

  return kp * error + ki * integral + kd * derivative;
}