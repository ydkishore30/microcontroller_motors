#pragma once
#include <Arduino.h>
#include "ICommandSource.h"
#include "../hal/IEncoder.h"

// Serial protocol for ros2_control's my_hardware.cpp:
//   ROS -> ESP32: "leftSpeed rightSpeed\n" (floats in [-1,1]) - fire-and-
//                 forget velocity command, no reply.
//   ROS -> ESP32: "E\n" -> ESP32 replies "E <leftTicks> <rightTicks>\n"
//   ROS -> ESP32: "R\n" -> ESP32 replies "R <leftRPM> <rightRPM>\n"
// E/R are synchronous queries - nothing else may print to Serial between
// receiving one and replying, or ros2_control's line-timeout read would
// desync (see SerialTelemetryPublisher, which is NOT used alongside this
// class for that reason).
class SerialCommandSource : public ICommandSource {
public:
  SerialCommandSource(IEncoder& leftEncoder, IEncoder& rightEncoder);

  void begin() override;
  void poll() override;

  float getLeftCommand() const override;
  float getRightCommand() const override;

private:
  IEncoder& leftEncoder;
  IEncoder& rightEncoder;

  String input = "";
  float cmdL = 0.0f;
  float cmdR = 0.0f;

  // Rolling RPM estimate, refreshed every poll() so an "R" query always
  // has a recent value on hand rather than computing dt against whenever
  // the last query happened to arrive.
  long prevLeftTicks = 0;
  long prevRightTicks = 0;
  unsigned long lastVelocityUpdate = 0;
  float leftRPM = 0.0f;
  float rightRPM = 0.0f;

  void updateVelocityEstimate();
  void handleLine(const String& line);
};
