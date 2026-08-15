#pragma once
#include <std_msgs/msg/float32_multi_array.h>
#include "ITelemetryPublisher.h"
#include "MicroRosNode.h"

// Publishes a Float32MultiArray on /telemetry:
// [leftTicks, rightTicks, cmdL, cmdR, accelX, accelY, accelZ,
//  gyroX, gyroY, gyroZ, busVoltage, current]
class MicroRosTelemetryPublisher : public ITelemetryPublisher {
public:
  explicit MicroRosTelemetryPublisher(MicroRosNode& node, unsigned long intervalMs = 500);

  void begin() override;
  void publish(long leftTicks, long rightTicks,
                float cmdL, float cmdR,
                IImuSensor& imu, ICurrentSensor& currentSensor) override;

private:
  static const size_t FIELD_COUNT = 12;

  MicroRosNode& node;
  unsigned long intervalMs;
  unsigned long lastPublish = 0;

  rcl_publisher_t publisher;
  std_msgs__msg__Float32MultiArray msg;
  float msgBuffer[FIELD_COUNT];
};
