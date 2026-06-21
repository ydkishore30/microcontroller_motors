#pragma once
#include <Arduino.h>
#include "ITelemetryPublisher.h"

// Prints a debug line over Serial, throttled to one line per interval.
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
};
