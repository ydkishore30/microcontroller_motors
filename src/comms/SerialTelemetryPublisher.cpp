#include "SerialTelemetryPublisher.h"

SerialTelemetryPublisher::SerialTelemetryPublisher(unsigned long intervalMs)
  : intervalMs(intervalMs) {}

void SerialTelemetryPublisher::begin() {}

void SerialTelemetryPublisher::publish(long leftTicks, long rightTicks,
                                        float cmdL, float cmdR,
                                        IImuSensor& imu, ICurrentSensor& currentSensor) {
  unsigned long now = millis();
  if (now - lastPublish < intervalMs) return;

  float dt = (now - lastPublish) / 1000.0f;
  lastPublish = now;

  float leftVel = dt > 0.0f ? (leftTicks - prevLeftTicks) / dt : 0.0f;
  float rightVel = dt > 0.0f ? (rightTicks - prevRightTicks) / dt : 0.0f;
  prevLeftTicks = leftTicks;
  prevRightTicks = rightTicks;

  Serial.print(leftTicks);
  Serial.print(",");
  Serial.print(rightTicks);
  Serial.print(",");
  Serial.print(leftVel);
  Serial.print(",");
  Serial.print(rightVel);

  // Trailing columns beyond the first 4 - ros2_control's CSV parser
  // (my_hardware.cpp) only reads indices 0-3, so these are ignored there
  // and just give visibility for manual serial monitoring.
  Serial.print(",");
  Serial.print(imu.getAccelX());
  Serial.print(",");
  Serial.print(imu.getAccelY());
  Serial.print(",");
  Serial.print(imu.getAccelZ());
  Serial.print(",");
  Serial.print(imu.getGyroX());
  Serial.print(",");
  Serial.print(imu.getGyroY());
  Serial.print(",");
  Serial.print(imu.getGyroZ());
  Serial.print(",");
  Serial.print(currentSensor.getBusVoltage());
  Serial.print(",");
  Serial.println(currentSensor.getCurrent());
}
