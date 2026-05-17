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
#define R_RPWM 33
#define R_LPWM 32
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
  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;

  // Run the control loop at roughly 50 Hz to keep timing stable
  if (dt >= 0.02) {
    lastTime = now;

    // Example: send a velocity command to the differential drive
    // drive.setVelocity(100.0, 0.0, dt);

    // Print encoder ticks for debugging and performance monitoring
    Serial.print("L: ");
    Serial.print(leftEncoder.getTicks());
    Serial.print(" R: ");
    Serial.println(rightEncoder.getTicks());
  }
}
