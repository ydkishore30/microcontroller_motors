#include "MicroRosWifiTransport.h"
#include <micro_ros_platformio.h>

void MicroRosWifiTransport::configure(const char* ssid, const char* password,
                                       const char* agentIp, uint16_t agentPort) {
  this->ssid = ssid;
  this->password = password;
  this->agentIp = agentIp;
  this->agentPort = agentPort;
}

void MicroRosWifiTransport::begin() {
  IPAddress agentAddress;
  agentAddress.fromString(agentIp);

  set_microros_wifi_transports((char*)ssid, (char*)password, agentAddress, agentPort);

  delay(2000); // allow the WiFi/agent connection to settle
}
