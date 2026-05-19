#include "hal/Motor.h"
#include "hal/Encoder.h"
#include "control/MotorController.h"

#include "kinematics/DifferentialDrive.h"

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

// Motor controllers (wrap motor + encoder)
MotorController leftController(leftMotor, leftEncoder);
MotorController rightController(rightMotor, rightEncoder);

// Differential drive
DifferentialDrive drive(leftController, rightController);

unsigned long lastTime = 0;

// ==============================
// GLOBAL STATE
// ==============================

float linear = 0.0f;
float angular = 0.0f;
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
  // format: "linear angular"
  // example: "100 0" (forward)
  //          "0 50"  (rotate)
  // -------------------------

  static String input = "";

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {

      input.trim();

      int space = input.indexOf(' ');

      if (space > 0) {
        linear  = input.substring(0, space).toFloat();
        angular = input.substring(space + 1).toFloat();
      }

      input = "";
    } else {
      input += c;
    }
  }

  // -------------------------
  // 2. COMPUTE dt
  // -------------------------
  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0f;
  lastTime = now;

  // Safety fallback
  if (dt <= 0) dt = 0.01f;

  // -------------------------
  // 3. APPLY DIFFERENTIAL DRIVE
  // -------------------------

  drive.setVelocity(linear, angular, dt);

  // -------------------------
  // 4. DEBUG OUTPUT
  // -------------------------

  static int counter = 0;
  counter++;

  if (counter % 50 == 0) {
    Serial.print("L ticks: ");
    Serial.print(leftEncoder.getTicks());

    Serial.print(" | R ticks: ");
    Serial.print(rightEncoder.getTicks());

    Serial.print(" | Lin: ");
    Serial.print(linear);

    Serial.print(" | Ang: ");
    Serial.println(angular);
  }
}