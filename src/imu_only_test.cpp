#include "hal/MPU6050.h"
#include "hal/I2CBusRecovery.h"

#define I2C_SDA 21
#define I2C_SCL 22

MPU6050 imu;

void setup() {
  Serial.begin(115200);
  delay(500);

  recoverI2CBus(I2C_SDA, I2C_SCL);
  Wire.begin();

  Serial.println("IMU ONLY TEST: nothing else running (no motors/encoders/other sensors)");

  if (!imu.begin()) {
    Serial.println("IMU begin FAILED - check wiring/address");
  } else {
    Serial.println("IMU begin OK");
  }
}

void loop() {
  imu.update();

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    lastPrint = millis();
    Serial.print("Accel: ");
    Serial.print(imu.getAccelX(), 4);
    Serial.print(",");
    Serial.print(imu.getAccelY(), 4);
    Serial.print(",");
    Serial.print(imu.getAccelZ(), 4);

    Serial.print(" | Gyro: ");
    Serial.print(imu.getGyroX(), 4);
    Serial.print(",");
    Serial.print(imu.getGyroY(), 4);
    Serial.print(",");
    Serial.print(imu.getGyroZ(), 4);

    Serial.print(" | Temp: ");
    Serial.println(imu.getTemperature(), 2);
  }
}
