#include "hal/Motor.h"
#include "hal/Encoder.h"
#include "hal/MPU6050.h"
#include "hal/INA226.h"
#include "control/MotorController.h"
#include "control/PID.h"
#include "comms/ICommandSource.h"
#include "comms/ITelemetryPublisher.h"
#include "Config.h"

#ifdef USE_MICRO_ROS
#include "comms/MicroRosNode.h"
#include "comms/MicroRosCommandSource.h"
#include "comms/MicroRosTelemetryPublisher.h"
#ifdef MICRO_ROS_TRANSPORT_ARDUINO_SERIAL
#include "comms/MicroRosSerialTransport.h"
#else
#include "comms/MicroRosWifiTransport.h"
#include "RuntimeConfig.h"
#endif
#else
#include "comms/SerialCommandSource.h"
#include "comms/SerialTelemetryPublisher.h"
#endif

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

PID leftPid(1.0, 0.0, 0.1);
PID rightPid(1.0, 0.0, 0.1);

MotorController leftController(leftMotor, leftEncoder, leftPid);
MotorController rightController(rightMotor, rightEncoder, rightPid);

MPU6050 imu;
INA226 currentSensor;

// Comms transport - selected at compile time. The rest of setup()/loop()
// only ever touches the ICommandSource/ITelemetryPublisher interfaces,
// so swapping transports requires no changes below.
#ifdef USE_MICRO_ROS
MicroRosNode microRosNode(1); // 1 executor handle: the wheel_cmd subscription
MicroRosCommandSource concreteCommandSource(microRosNode);
MicroRosTelemetryPublisher concreteTelemetryPublisher(microRosNode);
#ifdef MICRO_ROS_TRANSPORT_ARDUINO_SERIAL
MicroRosSerialTransport microRosTransport(Serial);
#else
MicroRosWifiTransport microRosTransport;
RuntimeConfig runtimeConfig;
#endif
#else
SerialCommandSource concreteCommandSource;
SerialTelemetryPublisher concreteTelemetryPublisher;
#endif

ICommandSource& commandSource = concreteCommandSource;
ITelemetryPublisher& telemetryPublisher = concreteTelemetryPublisher;

void setup() {
  Serial.begin(115200);

  leftMotor.begin();
  rightMotor.begin();

  leftEncoder.begin(isrLeft);
  rightEncoder.begin(isrRight);

  Wire.begin();

  if (!imu.begin()) {
    Serial.println("IMU init failed");
  }

  if (!currentSensor.begin(SHUNT_RESISTOR_OHMS, MAX_EXPECTED_CURRENT_A)) {
    Serial.println("Current sensor init failed");
  }

#ifdef USE_MICRO_ROS
#ifdef MICRO_ROS_TRANSPORT_ARDUINO_SERIAL
  microRosNode.begin(microRosTransport, "motor_controller_node");
#else
  if (!runtimeConfig.load()) {
    runtimeConfig.promptAndWaitForSerialConfig(); // blocks, then restarts
  }

  microRosTransport.configure(runtimeConfig.getSsid(), runtimeConfig.getPassword(),
                               runtimeConfig.getAgentIp(), runtimeConfig.getAgentPort());
  microRosNode.begin(microRosTransport, "motor_controller_node");
#endif
#endif

  commandSource.begin();
  telemetryPublisher.begin();
}

void loop() {

  commandSource.poll();

#ifdef USE_MICRO_ROS
#ifndef MICRO_ROS_TRANSPORT_ARDUINO_SERIAL
  runtimeConfig.poll(); // allows re-provisioning via "CONFIG ..." at any time
#endif
  microRosNode.spinSome(10);
#endif

  // ==========================
  // OPEN LOOP MOTOR CONTROL
  // ==========================

  leftController.setOpenLoop(commandSource.getLeftCommand());
  rightController.setOpenLoop(commandSource.getRightCommand());

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
