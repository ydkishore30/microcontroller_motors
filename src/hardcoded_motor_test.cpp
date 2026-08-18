#include "hal/Motor.h"

// Isolated diagnostic: drives both motors at a fixed speed unconditionally
// from boot, with zero dependency on Serial/command parsing working. Rules
// out "commands aren't being received" as a variable - if the motors still
// don't move with this, the problem is definitively downstream of the
// ESP32 (driver board, wiring, or motor), not firmware/serial.

#define L_PWM 25
#define L_DIR 26
#define R_PWM 27
#define R_DIR 14

Motor leftMotor(L_PWM, L_DIR, 0);
Motor rightMotor(R_PWM, R_DIR, 1);

void setup() {
  Serial.begin(115200);
  Serial.println("HARDCODED MOTOR TEST: driving both motors at fixed speed, no serial input needed");

  leftMotor.begin();
  rightMotor.begin();
}

void loop() {
  leftMotor.setSpeed(0.3f);
  rightMotor.setSpeed(0.3f);

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    lastPrint = millis();
    Serial.println("Driving at 0.3 speed...");
  }
}
