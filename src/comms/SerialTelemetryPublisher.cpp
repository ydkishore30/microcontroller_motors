#include "SerialTelemetryPublisher.h"

SerialTelemetryPublisher::SerialTelemetryPublisher(unsigned long intervalMs)
  : intervalMs(intervalMs) {}

void SerialTelemetryPublisher::begin() {}

void SerialTelemetryPublisher::publish(long leftTicks, long rightTicks,
                                        float cmdL, float cmdR,
                                        IImuSensor& imu, ICurrentSensor& currentSensor) {
  if (millis() - lastPublish < intervalMs) return;
  lastPublish = millis();

  Serial.print("L ticks: ");
  Serial.print(leftTicks);

  Serial.print(" | R ticks: ");
  Serial.print(rightTicks);

  Serial.print(" | CmdL: ");
  Serial.print(cmdL);

  Serial.print(" | CmdR: ");
  Serial.print(cmdR);

  Serial.print(" | Accel: ");
  Serial.print(imu.getAccelX());
  Serial.print(",");
  Serial.print(imu.getAccelY());
  Serial.print(",");
  Serial.print(imu.getAccelZ());

  Serial.print(" | Gyro: ");
  Serial.print(imu.getGyroX());
  Serial.print(",");
  Serial.print(imu.getGyroY());
  Serial.print(",");
  Serial.print(imu.getGyroZ());

  Serial.print(" | Bus V: ");
  Serial.print(currentSensor.getBusVoltage());

  Serial.print(" | Current: ");
  Serial.println(currentSensor.getCurrent());
}
