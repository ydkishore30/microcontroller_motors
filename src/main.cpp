#include "hal/Motor.h"
#include "hal/Encoder.h"
#include "hal/MPU6050.h"
#include "hal/INA226.h"
#include "hal/I2CBusRecovery.h"
#include "control/MotorController.h"
#include "control/PID.h"
#include "comms/ICommandSource.h"
#include "comms/ITelemetryPublisher.h"
#include "Config.h"

#ifdef USE_MICRO_ROS
#include "comms/MicroRosNode.h"
#include "comms/MicroRosSerialTransport.h"
#include "comms/MicroRosCommandSource.h"
#include "comms/MicroRosTelemetryPublisher.h"
#else
#include "comms/SerialCommandSource.h"
#include "comms/SerialTelemetryPublisher.h"
#endif

// LEFT MOTOR (SmartElex 15D Independent PWM Mode: S1 = speed, D1 = direction)
#define R_PWM 25
#define R_DIR 26
#define ENC_L_A 34
#define ENC_L_B 35

// RIGHT MOTOR (S2 = speed, D2 = direction)
#define L_PWM 27
#define L_DIR 14
#define ENC_R_A 32
#define ENC_R_B 33

// I2C bus (shared by MPU6050 and INA226)
#define I2C_SDA 21
#define I2C_SCL 22

Motor leftMotor(L_PWM, L_DIR, 0);
Motor rightMotor(R_PWM, R_DIR, 1);

Encoder leftEncoder(ENC_L_A, ENC_L_B);
Encoder rightEncoder(ENC_R_A, ENC_R_B);

void IRAM_ATTR isrLeft() {
  leftEncoder.handleA();
}

void IRAM_ATTR isrRight() {
  rightEncoder.handleA();
}

// Pure-P (kp=0.0003) settles far below the commanded target (~32 RPM
// actual vs ~80 RPM target at full command) - that's not a display
// issue, it's inherent to P-only control: the wheel settles wherever
// kp*error balances against friction, which sits well under target no
// matter how long you wait. ki is required to close that gap. It does
// reintroduce a brief startup jerk from integral windup during stiction
// (see PID.cpp) but that jerk is cosmetic and already confirmed
// acceptable, while the steady-state shortfall is not.
PID leftPid(0.0003, 0.0001, 0.0000);
PID rightPid(0.0003, 0.0001, 0.0000);



MotorController leftController(leftMotor, leftEncoder, leftPid);
MotorController rightController(rightMotor, rightEncoder, rightPid);

MPU6050 imu;
INA226 currentSensor;

#ifdef USE_MICRO_ROS
MicroRosSerialTransport microRosTransport;
MicroRosNode microRosNode(1);  // 1 executor slot: the /wheel_cmd subscription
MicroRosCommandSource concreteCommandSource(microRosNode);
MicroRosTelemetryPublisher concreteTelemetryPublisher(microRosNode);
#else
SerialCommandSource concreteCommandSource(leftEncoder, rightEncoder);
SerialTelemetryPublisher concreteTelemetryPublisher;
#endif

ICommandSource& commandSource = concreteCommandSource;
ITelemetryPublisher& telemetryPublisher = concreteTelemetryPublisher;

float rampedTargetLeft = 0.0f;
float rampedTargetRight = 0.0f;

// Moves current towards target by at most maxDelta, for a smooth
// acceleration ramp instead of an instant step in the PID's target.
float rampTowards(float current, float target, float maxDelta) {
  float diff = target - current;
  if (diff > maxDelta) diff = maxDelta;
  else if (diff < -maxDelta) diff = -maxDelta;
  return current + diff;
}

void setup() {
  // set_microros_serial_transports() only registers read/write callbacks on
  // the given Stream - it does not call Serial.begin() itself, so this is
  // still required even in the micro-ROS build.
  Serial.begin(115200);

#ifdef USE_MICRO_ROS
  // From here on, no other code may Serial.print to this stream - that
  // would corrupt the XRCE-DDS binary protocol microRosNode.begin() sets up.
  microRosNode.begin(microRosTransport, "motor_controller_node");
#endif

  leftMotor.begin();
  rightMotor.begin();

  leftEncoder.begin(isrLeft);
  rightEncoder.begin(isrRight);

#if ENABLE_I2C_SENSORS
  recoverI2CBus(I2C_SDA, I2C_SCL);
  Wire.begin();

  if (!imu.begin()) {
#ifndef USE_MICRO_ROS
    Serial.println("IMU init failed");
#endif
  }

  if (!currentSensor.begin(SHUNT_RESISTOR_OHMS, MAX_EXPECTED_CURRENT_A)) {
#ifndef USE_MICRO_ROS
    Serial.println("Current sensor init failed");
#endif
  }
#endif

  commandSource.begin();
  telemetryPublisher.begin();
}

void loop() {

#ifdef USE_MICRO_ROS
  // Processes incoming /wheel_cmd messages (MicroRosCommandSource::onMessage)
  // and drives the /telemetry publisher's executor callbacks.
  microRosNode.spinSome(10);
#endif

  commandSource.poll();

  // ==========================
  // VELOCITY CONTROL
  // Toggled by USE_CLOSED_LOOP_PID in Config.h. Disabled, not deleted:
  // both the open-loop and closed-loop code paths stay here so either
  // can be switched back on with a single define, no logic rewritten.
  // ==========================

  const unsigned long CONTROL_INTERVAL_MS = 100;
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();

  if (now - lastUpdate >= CONTROL_INTERVAL_MS) {
    float dt = (now - lastUpdate) / 1000.0f;
    lastUpdate = now;

    float leftCommand = commandSource.getLeftCommand();
    float rightCommand = commandSource.getRightCommand();

#if USE_CLOSED_LOOP_PID
    // Command is normalized [-1, 1]; scaled to a target speed in
    // ticks/sec and tracked via PID using encoder feedback.
    float maxDelta = MAX_ACCEL_TICKS_PER_SEC2 * dt;

    // A zero command means stop, not "target speed 0 ticks/sec" - the
    // latter would let any leftover PID integral keep weakly driving
    // the motor until it unwinds, and would ramp the stop instead of
    // cutting power immediately. Reset and cut power right away.
    if (leftCommand == 0.0f) {
      leftController.stop();
      rampedTargetLeft = 0.0f;
    } else {
      rampedTargetLeft = rampTowards(rampedTargetLeft, leftCommand * MAX_WHEEL_SPEED_TICKS_PER_SEC, maxDelta);
      leftController.update(rampedTargetLeft, dt);
    }

    if (rightCommand == 0.0f) {
      rightController.stop();
      rampedTargetRight = 0.0f;
    } else {
      rampedTargetRight = rampTowards(rampedTargetRight, rightCommand * MAX_WHEEL_SPEED_TICKS_PER_SEC, maxDelta);
      rightController.update(rampedTargetRight, dt);
    }
#else
    // Command is normalized [-1, 1] and applied directly as motor
    // power, ramped for a smooth start instead of stepping instantly.
    // Same ramp duration as the closed-loop path above (0->full in
    // MAX_WHEEL_SPEED_TICKS_PER_SEC / MAX_ACCEL_TICKS_PER_SEC2 seconds),
    // just expressed in command units since there's no speed target.
    float maxDeltaCommand = (MAX_ACCEL_TICKS_PER_SEC2 / MAX_WHEEL_SPEED_TICKS_PER_SEC) * dt;

    // A zero command means stop immediately, not "ramp down to zero" -
    // cut power right away instead of coasting through the ramp.
    if (leftCommand == 0.0f) {
      rampedTargetLeft = 0.0f;
      leftController.setOpenLoop(0.0f);
    } else {
      rampedTargetLeft = rampTowards(rampedTargetLeft, leftCommand, maxDeltaCommand);
      leftController.setOpenLoop(rampedTargetLeft);
    }

    if (rightCommand == 0.0f) {
      rampedTargetRight = 0.0f;
      rightController.setOpenLoop(0.0f);
    } else {
      rampedTargetRight = rampTowards(rampedTargetRight, rightCommand, maxDeltaCommand);
      rightController.setOpenLoop(rampedTargetRight);
    }
#endif
  }

  // ==========================
  // SENSORS
  // ==========================

#if ENABLE_I2C_SENSORS
  imu.update();
  currentSensor.update();
#endif

  // ==========================
  // TELEMETRY
  // ==========================

#ifdef USE_MICRO_ROS
  // Under plain Serial, telemetry is pulled on-demand via SerialCommandSource's
  // "E"/"R" query-response (matching my_hardware.cpp) - an unprompted publish
  // here would inject an extra line between a query and its reply and desync
  // ros2_control's line-timeout read. Micro-ROS publishes over its own
  // transport, so no such conflict there.
  telemetryPublisher.publish(
    leftEncoder.getTicks(), rightEncoder.getTicks(),
    commandSource.getLeftCommand(), commandSource.getRightCommand(),
    imu, currentSensor);
#endif
}
