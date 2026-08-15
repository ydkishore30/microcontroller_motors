#include "MicroRosCommandSource.h"
#include <Arduino.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

MicroRosCommandSource* MicroRosCommandSource::instance = nullptr;

MicroRosCommandSource::MicroRosCommandSource(MicroRosNode& node) : node(node) {
  instance = this;
}

void MicroRosCommandSource::begin() {
  msg.data.data = msgBuffer;
  msg.data.size = 0;
  msg.data.capacity = 2;

  rclc_subscription_init_default(
    &subscription,
    &node.getNode(),
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32MultiArray),
    "wheel_cmd");

  rclc_executor_add_subscription(
    &node.getExecutor(), &subscription, &msg, &MicroRosCommandSource::onMessage, ON_NEW_DATA);
}

void MicroRosCommandSource::poll() {
  // Command values are updated asynchronously by onMessage() whenever
  // MicroRosNode::spinSome() processes a new message; nothing to do here.
}

void MicroRosCommandSource::onMessage(const void* msgIn) {
  if (!instance) return;

  auto* received = static_cast<const std_msgs__msg__Float32MultiArray*>(msgIn);

  if (received->data.size >= 2) {
    instance->cmdL = constrain(received->data.data[0], -1.0f, 1.0f);
    instance->cmdR = constrain(received->data.data[1], -1.0f, 1.0f);
  }
}

float MicroRosCommandSource::getLeftCommand() const { return cmdL; }
float MicroRosCommandSource::getRightCommand() const { return cmdR; }
