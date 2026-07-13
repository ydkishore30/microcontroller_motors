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

    // Also cap the integral term's own contribution directly. Output
    // saturation alone doesn't catch a stalled wheel (e.g. motor stiction
    // at a tiny ramped-up target speed): output stays small and unsaturated
    // while the wheel sits still, so integral grows unchecked until it
    // finally overcomes stiction and the wheel lurches/overshoots, which
    // can briefly reverse direction before settling.
    //
    // The cap must still allow ki*integral to reach close to full output
    // on its own: at steady state error->0, so kp*error->0 and the entire
    // sustained output needed to hold speed against friction/load comes
    // from this term. Capping it too low (e.g. 0.5) silently caps the
    // max reachable steady-state speed well below the commanded target,
    // even though the wheel could go faster - which looks identical to a
    // tuning problem but is actually this clamp being too strict.
    if (ki != 0.0f) {
      float maxIntegral = 0.95f / ki;
      if (integral > maxIntegral) integral = maxIntegral;
      else if (integral < -maxIntegral) integral = -maxIntegral;
    }
  }

  return output;
}

void PID::reset() {
  integral = 0;
  prevError = 0;
}