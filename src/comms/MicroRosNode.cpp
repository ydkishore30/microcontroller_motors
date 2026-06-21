#include "MicroRosNode.h"
#include <Arduino.h>
#include <rcl/error_handling.h>

namespace {
  void haltOnError() {
    Serial.println("micro-ROS error - halting");
    while (true) {
      delay(100);
    }
  }
}

#define RCCHECK(fn) { rcl_ret_t rc = (fn); if (rc != RCL_RET_OK) { haltOnError(); } }

MicroRosNode::MicroRosNode(size_t executorHandles)
  : executorHandles(executorHandles) {}

void MicroRosNode::begin(IMicroRosTransport& transport, const char* nodeName) {
  transport.begin();

  allocator = rcl_get_default_allocator();

  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  RCCHECK(rclc_node_init_default(&node, nodeName, "", &support));
  RCCHECK(rclc_executor_init(&executor, &support.context, executorHandles, &allocator));
}

rcl_node_t& MicroRosNode::getNode() { return node; }
rclc_executor_t& MicroRosNode::getExecutor() { return executor; }
rcl_allocator_t& MicroRosNode::getAllocator() { return allocator; }

void MicroRosNode::spinSome(int64_t timeoutMs) {
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(timeoutMs));
}
