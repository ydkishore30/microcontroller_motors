#ifndef PID_H
#define PID_H

#include "IController.h"

class PID : public IController {
  float kp, ki, kd;
  float prevError = 0;
  float integral = 0;

public:
  PID(float p, float i, float d);
  float compute(float target, float current, float dt) override;
};

#endif