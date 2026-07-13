#pragma once
#include <Arduino.h>
#include "ITelemetryPublisher.h"

// Prints "leftTicks,rightTicks,leftVel,rightVel\n" over Serial (ticks/sec),
// throttled to one line per interval. CSV-only, matching what
// ros2_control's hardware interface (my_hardware.cpp) parses - IMU and
// current sensor data are not sent for now.
class SerialTelemetryPublisher : public ITelemetryPublisher {
public:
  explicit SerialTelemetryPublisher(unsigned long intervalMs = 500);

  void begin() override;
  void publish(long leftTicks, long rightTicks,
                float cmdL, float cmdR,
                IImuSensor& imu, ICurrentSensor& currentSensor) override;

private:
  unsigned long intervalMs;
  unsigned long lastPublish = 0;
  long prevLeftTicks = 0;
  long prevRightTicks = 0;
};
