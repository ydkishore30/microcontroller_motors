#include "hal/Motor.h"
#include "hal/Encoder.h"
#include "control/MotorController.h"
#include "control/PID.h"

// Isolated hardware test: motors + encoders + velocity only. No IMU, no
// current sensor, no micro-ROS. Used to check encoder counting and
// computed wheel velocity in isolation from the sensors that are known
// to be unreliable right now.

// LEFT MOTOR
#define L_PWM 25
#define L_DIR 26
#define ENC_L_A 32
#define ENC_L_B 33

// RIGHT MOTOR
#define R_PWM 27
#define R_DIR 14
#define ENC_R_A 18
#define ENC_R_B 19

Motor leftMotor(L_PWM, L_DIR);
Motor rightMotor(R_PWM, R_DIR);

Encoder leftEncoder(ENC_L_A, ENC_L_B);
Encoder rightEncoder(ENC_R_A, ENC_R_B);

void IRAM_ATTR isrLeft() {
  leftEncoder.handleA();
}

void IRAM_ATTR isrRight() {
  rightEncoder.handleA();
}

PID leftPid(1.0, 0.0, 0.1);
PID rightPid(1.0, 0.0, 0.1);

MotorController leftController(leftMotor, leftEncoder, leftPid);
MotorController rightController(rightMotor, rightEncoder, rightPid);

String input = "";
float cmdL = 0.0f;
float cmdR = 0.0f;

void setup() {
  Serial.begin(115200);

  leftMotor.begin();
  rightMotor.begin();

  leftEncoder.begin(isrLeft);
  rightEncoder.begin(isrRight);

  Serial.println("MOTOR/ENCODER VELOCITY TEST (no IMU, no current sensor)");
  Serial.println("Send: leftSpeed rightSpeed   (range -1.0 to 1.0, e.g. 0.5 0.5)");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      input.trim();

      int space = input.indexOf(' ');
      if (space > 0) {
        cmdL = constrain(input.substring(0, space).toFloat(), -1.0f, 1.0f);
        cmdR = constrain(input.substring(space + 1).toFloat(), -1.0f, 1.0f);
      }

      input = "";
    } else {
      input += c;
    }
  }

  leftController.setOpenLoop(cmdL);
  rightController.setOpenLoop(cmdR);

  static unsigned long lastPrint = 0;
  unsigned long now = millis();

  if (now - lastPrint >= 200) {
    float dt = (now - lastPrint) / 1000.0f;
    lastPrint = now;

    float leftSpeed = leftController.getSpeed(dt);   // ticks/sec
    float rightSpeed = rightController.getSpeed(dt); // ticks/sec

    Serial.print("CmdL=");
    Serial.print(cmdL);
    Serial.print(" CmdR=");
    Serial.print(cmdR);

    Serial.print(" | L ticks=");
    Serial.print(leftEncoder.getTicks());
    Serial.print(" Lvel(ticks/s)=");
    Serial.print(leftSpeed);

    Serial.print(" | R ticks=");
    Serial.print(rightEncoder.getTicks());
    Serial.print(" Rvel(ticks/s)=");
    Serial.println(rightSpeed);
  }
}
