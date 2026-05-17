#include "hal/Motor.h"
#include "hal/Encoder.h"
#include "control/MotorController.h"

// LEFT MOTOR
#define L_RPWM 25
#define L_LPWM 26
#define ENC_L_A 14
#define ENC_L_B 27

// RIGHT MOTOR
#define R_RPWM 32
#define R_LPWM 33
#define ENC_R_A 35
#define ENC_R_B 34

Motor leftMotor(L_RPWM, L_LPWM, 0, 1);
Motor rightMotor(R_RPWM, R_LPWM, 2, 3);

Encoder leftEncoder(ENC_L_A, ENC_L_B);
Encoder rightEncoder(ENC_R_A, ENC_R_B);

void IRAM_ATTR isrLeft() { leftEncoder.handleA(); }
void IRAM_ATTR isrRight() { rightEncoder.handleA(); }

unsigned long lastTime = 0;

// stored commands (IMPORTANT)
int cmdL = 0;
int cmdR = 0;

void setup() {
  Serial.begin(115200);

  leftMotor.begin();
  rightMotor.begin();

  leftEncoder.begin(isrLeft);
  rightEncoder.begin(isrRight);

  Serial.println("READY");
}

void loop() {

  // -------------------------
  // 1. READ SERIAL COMMAND
  // format: "L R"
  // -------------------------
  static String input = "";

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {

      input.trim();

      int space = input.indexOf(' ');

      if (space > 0) {
        cmdL = input.substring(0, space).toInt();
        cmdR = input.substring(space + 1).toInt();
      }

      input = "";
    } else {
      input += c;
    }
  }

  // -------------------------
  // 2. APPLY MOTOR OUTPUT
  // -------------------------
  leftMotor.setSpeed(cmdL);
  rightMotor.setSpeed(cmdR);

  // -------------------------
  // 3. DEBUG (slow)
  // -------------------------  
  static int counter = 0;
  counter++;

  if (counter % 50 == 0) {
    Serial.print("L:");
    Serial.print(leftEncoder.getTicks());
    Serial.print(" R:");
    Serial.println(rightEncoder.getTicks());
  }
}