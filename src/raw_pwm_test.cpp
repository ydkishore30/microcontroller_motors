#include <Arduino.h>

// Diagnostic: alternates which physical pin gets the PWM signal and
// which gets a static digital HIGH, for both channels, in case the
// PWM/DIR wires are actually crossed at the board's connector (its JST
// pinout is GND,D2,S2,D1,S1 - easy to swap S/D during rewiring). If
// neither orientation moves the motors, it's conclusively not a
// PWM/DIR mixup.

#define PIN_A_LEFT 25   // L_PWM or L_DIR depending on current orientation
#define PIN_B_LEFT 26   // the other one
#define PIN_A_RIGHT 27  // R_PWM or R_DIR depending on current orientation
#define PIN_B_RIGHT 14  // the other one

#define PWM_FREQUENCY_HZ 490
#define PWM_RESOLUTION_BITS 8
#define SWAP_INTERVAL_MS 5000

bool pwmOnA = true;  // true: PWM on *_A pins, HIGH on *_B pins. false: reversed.

void applyOrientation(bool pwmOnA) {
  // Detach both pins from LEDC and reset to plain digital first, since a
  // pin previously attached to a channel needs to be freed before the
  // other pin in this pair can take over that channel cleanly.
  ledcDetachPin(PIN_A_LEFT);
  ledcDetachPin(PIN_B_LEFT);
  ledcDetachPin(PIN_A_RIGHT);
  ledcDetachPin(PIN_B_RIGHT);

  int duty94Percent = (int)(0.94f * 255.0f);

  int pwmPinLeft  = pwmOnA ? PIN_A_LEFT  : PIN_B_LEFT;
  int highPinLeft = pwmOnA ? PIN_B_LEFT  : PIN_A_LEFT;
  int pwmPinRight  = pwmOnA ? PIN_A_RIGHT : PIN_B_RIGHT;
  int highPinRight = pwmOnA ? PIN_B_RIGHT : PIN_A_RIGHT;

  pinMode(highPinLeft, OUTPUT);
  pinMode(highPinRight, OUTPUT);
  digitalWrite(highPinLeft, HIGH);
  digitalWrite(highPinRight, HIGH);

  ledcSetup(0, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);
  ledcSetup(1, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS);
  ledcAttachPin(pwmPinLeft, 0);
  ledcAttachPin(pwmPinRight, 1);
  ledcWrite(0, duty94Percent);
  ledcWrite(1, duty94Percent);

  Serial.print(">>> Orientation: PWM(94%) on pins ");
  Serial.print(pwmPinLeft);
  Serial.print("/");
  Serial.print(pwmPinRight);
  Serial.print(", HIGH on pins ");
  Serial.print(highPinLeft);
  Serial.print("/");
  Serial.println(highPinRight);
}

void setup() {
  Serial.begin(115200);
  Serial.println("RAW PWM TEST: alternating PWM/DIR pin orientation every 5s");
  applyOrientation(pwmOnA);
}

void loop() {
  static unsigned long lastSwap = 0;
  if (millis() - lastSwap > SWAP_INTERVAL_MS) {
    lastSwap = millis();
    pwmOnA = !pwmOnA;
    applyOrientation(pwmOnA);
  }
}
