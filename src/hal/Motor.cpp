#include "Motor.h"

// SmartElex 15D Independent PWM Mode: input PWM duty below 8% reads as
// 0% motor speed (dead zone), 8%-94% maps to 8%-100% motor speed, and
// the board's own spec caps input PWM frequency at 490Hz - far slower
// than the 20kHz used on a direct-drive RPWM/LPWM board, since here the
// duty cycle is just a command sampled by the driver's own MCU rather
// than the raw MOSFET switching signal.
static const float MIN_DUTY_PERCENT = 8.0f;
static const float MAX_DUTY_PERCENT = 94.0f;
static const int PWM_FREQUENCY_HZ = 490;

Motor::Motor(int pwmPin, int dirPin, int pwmChannel)
  : pin_pwm(pwmPin), pin_dir(dirPin), ch_pwm(pwmChannel) {}

void Motor::begin() {
  pinMode(pin_dir, OUTPUT);

  ledcSetup(ch_pwm, PWM_FREQUENCY_HZ, 8);
  ledcAttachPin(pin_pwm, ch_pwm);
}

void Motor::setSpeed(float speed) {
  speed = constrain(speed, -1.0f, 1.0f);

  bool forward = speed >= 0.0f;
  if (inverted) forward = !forward;
  digitalWrite(pin_dir, forward ? HIGH : LOW);

  float magnitude = fabs(speed);

  if (magnitude == 0.0f) {
    ledcWrite(ch_pwm, 0);
    return;
  }

  float dutyPercent = MIN_DUTY_PERCENT + magnitude * (MAX_DUTY_PERCENT - MIN_DUTY_PERCENT);
  int duty = (int)(dutyPercent / 100.0f * 255.0f);
  ledcWrite(ch_pwm, duty);
}

void Motor::setInverted(bool inv) {
  inverted = inv;
}
