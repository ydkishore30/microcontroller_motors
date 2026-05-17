#include <Arduino.h>

// LEFT motor pins only
#define L_RPWM 25
#define L_LPWM 26

// LEFT encoder only
#define ENC_L_A 14
#define ENC_L_B 27

volatile long left_ticks = 0;

// ISR
void IRAM_ATTR ISR_left() {
  left_ticks++;
}

// PWM channels
#define CH_L_RPWM 0
#define CH_L_LPWM 1

void setupPWM() {
  ledcSetup(CH_L_RPWM, 2000, 8);
  ledcSetup(CH_L_LPWM, 2000, 8);

  ledcAttachPin(L_RPWM, CH_L_RPWM);
  ledcAttachPin(L_LPWM, CH_L_LPWM);
}

void setMotor(int speed) {
  speed = constrain(speed, -255, 255);

  if (speed >= 0) {
    ledcWrite(CH_L_RPWM, speed);
    ledcWrite(CH_L_LPWM, 0);
  } else {
    ledcWrite(CH_L_RPWM, 0);
    ledcWrite(CH_L_LPWM, -speed);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(ENC_L_A, INPUT_PULLUP);
  pinMode(ENC_L_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENC_L_A), ISR_left, RISING);

  setupPWM();
}

void loop() {
  // Forward
  setMotor(150);
  delay(2000);

  // Stop
  setMotor(0);
  delay(1000);

  noInterrupts();
  long ticks = left_ticks;
  interrupts();

  Serial.print("Left Ticks: ");
  Serial.println(ticks);
}