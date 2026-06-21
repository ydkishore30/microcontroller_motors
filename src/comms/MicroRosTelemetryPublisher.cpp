#include "MicroRosTelemetryPublisher.h"
#include <Arduino.h>
#include <rclc/rclc.h>

MicroRosTelemetryPublisher::MicroRosTelemetryPublisher(MicroRosNode& node, unsigned long intervalMs)
  : node(node), intervalMs(intervalMs) {}

void MicroRosTelemetryPublisher::begin() {
  msg.data.data = msgBuffer;
  msg.data.size = FIELD_COUNT;
  msg.data.capacity = FIELD_COUNT;

  rclc_publisher_init_default(
    &publisher,
    &node.getNode(),
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32MultiArray),
    "telemetry");
}

void MicroRosTelemetryPublisher::publish(long leftTicks, long rightTicks,
                                          float cmdL, float cmdR,
                                          IImuSensor& imu, ICurrentSensor& currentSensor) {
  if (millis() - lastPublish < intervalMs) return;
  lastPublish = millis();

  msgBuffer[0] = (float)leftTicks;
  msgBuffer[1] = (float)rightTicks;
  msgBuffer[2] = cmdL;
  msgBuffer[3] = cmdR;
  msgBuffer[4] = imu.getAccelX();
  msgBuffer[5] = imu.getAccelY();
  msgBuffer[6] = imu.getAccelZ();
  msgBuffer[7] = imu.getGyroX();
  msgBuffer[8] = imu.getGyroY();
  msgBuffer[9] = imu.getGyroZ();
  msgBuffer[10] = currentSensor.getBusVoltage();
  msgBuffer[11] = currentSensor.getCurrent();

  rcl_ret_t rc = rcl_publish(&publisher, &msg, NULL);
  if (rc != RCL_RET_OK) {
    Serial.println("telemetry publish failed");
  }
}
