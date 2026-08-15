#pragma once
#include <std_msgs/msg/float32_multi_array.h>
#include "ICommandSource.h"
#include "MicroRosNode.h"

// Subscribes to a Float32MultiArray on /wheel_cmd: [leftSpeed, rightSpeed],
// each in [-1.0, 1.0], matching the same wheel-command contract as
// SerialCommandSource.
class MicroRosCommandSource : public ICommandSource {
public:
  explicit MicroRosCommandSource(MicroRosNode& node);

  void begin() override;
  void poll() override;

  float getLeftCommand() const override;
  float getRightCommand() const override;

private:
  MicroRosNode& node;
  rcl_subscription_t subscription;
  std_msgs__msg__Float32MultiArray msg;
  float msgBuffer[2];

  float cmdL = 0.0f;
  float cmdR = 0.0f;

  // rclc subscription callbacks take no context pointer, so the single
  // instance registers itself here (one MicroRosCommandSource per program).
  static MicroRosCommandSource* instance;
  static void onMessage(const void* msgIn);
};
