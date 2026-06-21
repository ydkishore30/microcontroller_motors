#pragma once
#include <Arduino.h>
#include "IMicroRosTransport.h"

// micro-ROS agent reachable over WiFi/UDP. Credentials are supplied via
// configure() (typically from RuntimeConfig) rather than at construction,
// since they are only known once loaded from flash/Serial at runtime.
class MicroRosWifiTransport : public IMicroRosTransport {
public:
  void configure(const char* ssid, const char* password,
                 const char* agentIp, uint16_t agentPort);

  void begin() override;

private:
  const char* ssid = nullptr;
  const char* password = nullptr;
  const char* agentIp = nullptr;
  uint16_t agentPort = 0;
};
