#pragma once
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include "IMicroRosTransport.h"

// Owns the micro-ROS node lifecycle (allocator/support/node/executor)
// shared by the micro-ROS command source and telemetry publisher.
// Only one instance should exist per program. Depends on
// IMicroRosTransport rather than a concrete link, so WiFi/Serial/future
// transports can be swapped without changing this class.
class MicroRosNode {
public:
  // executorHandles is the number of subscriptions/timers that will be
  // registered with the executor (publishers do not need a slot).
  explicit MicroRosNode(size_t executorHandles);

  // Bring up the given transport, then bring up rcl/rclc on top of it.
  void begin(IMicroRosTransport& transport, const char* nodeName);

  rcl_node_t& getNode();
  rclc_executor_t& getExecutor();
  rcl_allocator_t& getAllocator();

  // Process pending callbacks. Must be called once per loop iteration.
  void spinSome(int64_t timeoutMs);

private:
  size_t executorHandles;

  rcl_allocator_t allocator;
  rclc_support_t support;
  rcl_node_t node;
  rclc_executor_t executor;
};
