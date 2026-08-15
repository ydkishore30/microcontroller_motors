#include "RuntimeConfig.h"

namespace {
  constexpr const char* NVS_NAMESPACE = "netcfg";
}

bool RuntimeConfig::load() {
  Preferences prefs;
  prefs.begin(NVS_NAMESPACE, true);

  ssid = prefs.getString("ssid", "");
  password = prefs.getString("pass", "");
  agentIp = prefs.getString("ip", "");
  agentPort = prefs.getUShort("port", 0);

  prefs.end();

  return ssid.length() > 0 && agentIp.length() > 0 && agentPort != 0;
}

void RuntimeConfig::save() {
  Preferences prefs;
  prefs.begin(NVS_NAMESPACE, false);

  prefs.putString("ssid", ssid);
  prefs.putString("pass", password);
  prefs.putString("ip", agentIp);
  prefs.putUShort("port", agentPort);

  prefs.end();
}

bool RuntimeConfig::parseAndSave(const String& line) {
  int firstSpace = line.indexOf(' ');
  if (firstSpace < 0) return false;

  int secondSpace = line.indexOf(' ', firstSpace + 1);
  if (secondSpace < 0) return false;

  int thirdSpace = line.indexOf(' ', secondSpace + 1);
  if (thirdSpace < 0) return false;

  String newSsid = line.substring(0, firstSpace);
  String newPassword = line.substring(firstSpace + 1, secondSpace);
  String newAgentIp = line.substring(secondSpace + 1, thirdSpace);
  uint16_t newAgentPort = (uint16_t)line.substring(thirdSpace + 1).toInt();

  if (newSsid.length() == 0 || newAgentIp.length() == 0 || newAgentPort == 0) return false;

  ssid = newSsid;
  password = newPassword;
  agentIp = newAgentIp;
  agentPort = newAgentPort;

  save();
  return true;
}

void RuntimeConfig::promptAndWaitForSerialConfig() {
  Serial.println("No saved WiFi/agent config found.");
  Serial.println("Enter: CONFIG <ssid> <password> <agentIp> <agentPort>");

  while (true) {
    while (Serial.available()) {
      char c = Serial.read();

      if (c == '\n') {
        inputLine.trim();

        if (inputLine.startsWith("CONFIG ")) {
          if (parseAndSave(inputLine.substring(7))) {
            Serial.println("Saved. Restarting...");
            delay(200);
            ESP.restart();
          } else {
            Serial.println("Invalid format. Enter: CONFIG <ssid> <password> <agentIp> <agentPort>");
          }
        }

        inputLine = "";
      } else {
        inputLine += c;
      }
    }
  }
}

void RuntimeConfig::poll() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      inputLine.trim();

      if (inputLine.startsWith("CONFIG ")) {
        if (parseAndSave(inputLine.substring(7))) {
          Serial.println("Saved. Restarting...");
          delay(200);
          ESP.restart();
        } else {
          Serial.println("Invalid format. Enter: CONFIG <ssid> <password> <agentIp> <agentPort>");
        }
      }

      inputLine = "";
    } else {
      inputLine += c;
    }
  }
}

const char* RuntimeConfig::getSsid() const { return ssid.c_str(); }
const char* RuntimeConfig::getPassword() const { return password.c_str(); }
const char* RuntimeConfig::getAgentIp() const { return agentIp.c_str(); }
uint16_t RuntimeConfig::getAgentPort() const { return agentPort; }
