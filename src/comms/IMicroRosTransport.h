#pragma once

// Abstraction for how the micro-ROS node reaches its agent. MicroRosNode
// depends on this rather than a concrete link (WiFi, Serial, ...), so new
// transports (e.g. a future combined WiFi+Serial fallback) can be added
// without changing MicroRosNode.
class IMicroRosTransport {
public:
  virtual ~IMicroRosTransport() = default;

  // Bring up the underlying link and register it with rmw_microros.
  // Must be called before MicroRosNode::begin().
  virtual void begin() = 0;
};
