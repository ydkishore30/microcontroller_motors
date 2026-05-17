#include "hal/Motor.h"
#include "hal/Encoder.h"
#include "control/MotorController.h"
#include "kinematics/DifferentialDrive.h"

// Hardware pin assignments for the left motor and encoder
#define L_RPWM 25
#define L_LPWM 26
#define ENC_L_A 14
#define ENC_L_B 27

// Hardware pin assignments for the right motor and encoder
#define R_RPWM 32
#define R_LPWM 33
#define ENC_R_A 34
#define ENC_R_B 35

// Motor objects encapsulate PWM channel setup and direction control
Motor leftMotor(L_RPWM, L_LPWM, 0, 1);
Motor rightMotor(R_RPWM, R_LPWM, 2, 3);

// Encoder objects manage tick counting from quadrature sensors
Encoder leftEncoder(ENC_L_A, ENC_L_B);
Encoder rightEncoder(ENC_R_A, ENC_R_B);

// Motor controllers combine motor output and encoder feedback
MotorController leftController(leftMotor, leftEncoder);
MotorController rightController(rightMotor, rightEncoder);

// Differential drive wrapper converts velocity commands into left/right wheel outputs
DifferentialDrive drive(leftController, rightController);

// Interrupt service routines must be short and only update encoder state
void IRAM_ATTR isrLeft() {
  leftEncoder.handleA();
}

void IRAM_ATTR isrRight() {
  rightEncoder.handleA();
}

unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);

  // Initialize motor PWM channels before use
  leftMotor.begin();
  rightMotor.begin();

  // Initialize encoder pins and attach interrupts
  leftEncoder.begin(isrLeft);
  rightEncoder.begin(isrRight);

  lastTime = millis();
}

void loop() {
  static unsigned long lastTime = 0;
  static String inputBuffer = "";

  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;

  // -----------------------------
  // 1. Read Serial safely (non-blocking)
  // -----------------------------
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n') {
      inputBuffer.trim();

      float linear = 0;
      float angular = 0;

      int spaceIndex = inputBuffer.indexOf(' ');

      if (spaceIndex > 0) {
        linear = inputBuffer.substring(0, spaceIndex).toFloat();
        angular = inputBuffer.substring(spaceIndex + 1).toFloat();
      }

      // Apply command immediately
      drive.setVelocity(linear, angular, dt);

      inputBuffer = "";  // clear buffer
    } else {
      inputBuffer += c;
    }
  }

  // -----------------------------
  // 2. Run control loop at 50Hz
  // -----------------------------
  if (dt >= 0.02) {
    lastTime = now;

    // -----------------------------
    // 3. Slow debug output (IMPORTANT FIX)
    // -----------------------------
    static int counter = 0;
    counter++;

    if (counter % 20 == 0) {   // prints every ~0.4 sec
      Serial.print("L:");
      Serial.print(leftEncoder.getTicks());

      Serial.print(" R:");
      Serial.println(rightEncoder.getTicks());
    }
  }
}