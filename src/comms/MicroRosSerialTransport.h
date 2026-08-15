#pragma once
#include <Arduino.h>
#include "IMicroRosTransport.h"

// micro-ROS agent reachable over a Stream (typically the USB Serial port).
// Note: once begin() is called, the given stream is owned by the micro-ROS
// XRCE-DDS link - any other code printing to the same Serial port (e.g.
// debug logs) will corrupt the binary protocol. Route debug output to a
// different UART (e.g. Serial2) when using this transport.
class MicroRosSerialTransport : public IMicroRosTransport {
public:
  explicit MicroRosSerialTransport(Stream& stream = Serial);

  void begin() override;

private:
  Stream& stream;
};
