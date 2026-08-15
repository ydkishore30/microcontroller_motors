#pragma once
#include "../hal/IImuSensor.h"
#include "../hal/ICurrentSensor.h"

// Abstraction for where telemetry goes. main.cpp depends on this rather
// than a concrete transport (Serial, micro-ROS), so the transport can
// be swapped via build configuration without changing the control loop.
class ITelemetryPublisher {
public:
  virtual ~ITelemetryPublisher() = default;

  // Perform one-time setup for the transport.
  virtual void begin() = 0;

  // Send the latest telemetry snapshot.
  virtual void publish(long leftTicks, long rightTicks,
                        float cmdL, float cmdR,
                        IImuSensor& imu, ICurrentSensor& currentSensor) = 0;
};
