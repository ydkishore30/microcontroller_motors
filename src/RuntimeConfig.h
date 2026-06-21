#pragma once
#include <Arduino.h>
#include <Preferences.h>

// Persists WiFi/agent credentials in NVS flash instead of source code,
// so secrets never end up in git history. Provisioned once over Serial
// with "CONFIG <ssid> <password> <agentIp> <agentPort>"; the device
// restarts to apply and reads the saved values on every later boot.
class RuntimeConfig {
public:
  // Load previously saved values. Returns false if none are saved yet.
  bool load();

  // Block on Serial until a valid CONFIG line is received, then save
  // and restart the device. Does not return.
  void promptAndWaitForSerialConfig();

  // Call once per loop iteration to allow re-provisioning at any time
  // by sending "CONFIG <ssid> <password> <agentIp> <agentPort>".
  void poll();

  const char* getSsid() const;
  const char* getPassword() const;
  const char* getAgentIp() const;
  uint16_t getAgentPort() const;

private:
  String ssid;
  String password;
  String agentIp;
  uint16_t agentPort = 0;
  String inputLine;

  void save();
  bool parseAndSave(const String& line);
};
