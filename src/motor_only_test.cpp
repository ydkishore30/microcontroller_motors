#include "hal/Motor.h"

// Bare minimum: read "L R" lines from Serial, drive motors directly.
// No encoders, no sensors, no MotorController/PID - just text in, PWM out.

#define L_PWM 25
#define L_DIR 26
#define R_PWM 27
#define R_DIR 14

Motor leftMotor(L_PWM, L_DIR);
Motor rightMotor(R_PWM, R_DIR);

String input = "";

void setup() {
  Serial.begin(115200);
  leftMotor.begin();
  rightMotor.begin();
  Serial.println("SIMPLE MOTOR TEST READY");
  Serial.println("Send: leftSpeed rightSpeed   (range -1.0 to 1.0, e.g. 0.5 0.5)");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      input.trim();

      int space = input.indexOf(' ');
      if (space > 0) {
        float left = input.substring(0, space).toFloat();
        float right = input.substring(space + 1).toFloat();

        left = constrain(left, -1.0f, 1.0f);
        right = constrain(right, -1.0f, 1.0f);

        leftMotor.setSpeed(left);
        rightMotor.setSpeed(right);

        Serial.print("Applied L=");
        Serial.print(left);
        Serial.print(" R=");
        Serial.println(right);
      }

      input = "";
    } else {
      input += c;
    }
  }
}
