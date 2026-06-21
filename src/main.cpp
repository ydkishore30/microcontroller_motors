#include "hal/Motor.h"
#include "hal/Encoder.h"
#include "hal/MPU6050.h"
#include "hal/INA226.h"
#include "hal/I2CBusRecovery.h"
#include "control/MotorController.h"
#include "control/PID.h"
#include "comms/ICommandSource.h"
#include "comms/ITelemetryPublisher.h"
#include "comms/SerialCommandSource.h"
#include "comms/SerialTelemetryPublisher.h"
#include "Config.h"

// LEFT MOTOR
#define L_RPWM 25
#define L_LPWM 26
#define ENC_L_A 32
#define ENC_L_B 33

// RIGHT MOTOR
#define R_RPWM 4
#define R_LPWM 16
#define ENC_R_A 18
#define ENC_R_B 19

// I2C bus (shared by MPU6050 and INA226)
#define I2C_SDA 21
#define I2C_SCL 22

Motor leftMotor(L_RPWM, L_LPWM, 0, 1);
Motor rightMotor(R_RPWM, R_LPWM, 2, 3);

Encoder leftEncoder(ENC_L_A, ENC_L_B);
Encoder rightEncoder(ENC_R_A, ENC_R_B);

void IRAM_ATTR isrLeft() {
  leftEncoder.handleA();
}

void IRAM_ATTR isrRight() {
  rightEncoder.handleA();
}

// Gains scaled for ticks/sec-magnitude error (target/current speed are
// in ticks/sec, output must land in [-1, 1] for Motor::setSpeed).
// Starting as pure P (I=D=0) deliberately: D on quantized/noisy encoder
// feedback caused oscillation, and I/D should only be added back in
// small increments after P alone is confirmed stable on real hardware.
PID leftPid(0.0007, 0.0, 0.0);
PID rightPid(0.0007, 0.0, 0.0);

MotorController leftController(leftMotor, leftEncoder, leftPid);
MotorController rightController(rightMotor, rightEncoder, rightPid);

MPU6050 imu;
INA226 currentSensor;

SerialCommandSource concreteCommandSource;
SerialTelemetryPublisher concreteTelemetryPublisher;

ICommandSource& commandSource = concreteCommandSource;
ITelemetryPublisher& telemetryPublisher = concreteTelemetryPublisher;

void setup() {
  Serial.begin(115200);

  leftMotor.begin();
  rightMotor.begin();

  leftEncoder.begin(isrLeft);
  rightEncoder.begin(isrRight);

  recoverI2CBus(I2C_SDA, I2C_SCL);
  Wire.begin();

  if (!imu.begin()) {
    Serial.println("IMU init failed");
  }

  if (!currentSensor.begin(SHUNT_RESISTOR_OHMS, MAX_EXPECTED_CURRENT_A)) {
    Serial.println("Current sensor init failed");
  }

  commandSource.begin();
  telemetryPublisher.begin();
}

void loop() {

  commandSource.poll();

  // ==========================
  // CLOSED LOOP VELOCITY CONTROL
  // Command is normalized [-1, 1]; scaled to a target speed in
  // ticks/sec and tracked via PID using encoder feedback. Rate-limited
  // to a fixed interval so dt stays consistent - calling this every raw
  // loop() iteration (dt of a few ms) makes the PID's derivative term
  // amplify encoder noise into large, unstable output swings.
  // ==========================

  const unsigned long CONTROL_INTERVAL_MS = 100;
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();

  if (now - lastUpdate >= CONTROL_INTERVAL_MS) {
    float dt = (now - lastUpdate) / 1000.0f;
    lastUpdate = now;

    float targetLeft = commandSource.getLeftCommand() * MAX_WHEEL_SPEED_TICKS_PER_SEC;
    float targetRight = commandSource.getRightCommand() * MAX_WHEEL_SPEED_TICKS_PER_SEC;

    leftController.update(targetLeft, dt);
    rightController.update(targetRight, dt);
  }

  // ==========================
  // SENSORS
  // ==========================

  imu.update();
  currentSensor.update();

  // ==========================
  // TELEMETRY
  // ==========================

  telemetryPublisher.publish(
    leftEncoder.getTicks(), rightEncoder.getTicks(),
    commandSource.getLeftCommand(), commandSource.getRightCommand(),
    imu, currentSensor);
}
