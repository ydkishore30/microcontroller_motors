#include <Arduino.h>
#include <Wire.h>
#include "hal/I2CBusRecovery.h"

// Isolated diagnostic: scans the full I2C address range and reports which
// addresses ACK, independent of any specific sensor driver. Used to tell
// apart "bus is completely dead" from "wrong address" or "one sensor bad".

#define I2C_SDA 21
#define I2C_SCL 22

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("I2C SCANNER: recovering bus...");
  recoverI2CBus(I2C_SDA, I2C_SCL);
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000);

  Serial.println("I2C SCANNER: scanning 0x01-0x7F...");
}

void loop() {
  int found = 0;

  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    uint8_t err = Wire.endTransmission();

    if (err == 0) {
      Serial.print("Device found at 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
      found++;
    } else if (err != 2) {
      // err == 2 is a plain NACK (nothing there) - anything else (timeout,
      // bus error) is worth surfacing since it points at a bus-level fault.
      Serial.print("Error ");
      Serial.print(err);
      Serial.print(" at 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
    }
  }

  Serial.print("Scan complete. Devices found: ");
  Serial.println(found);
  Serial.println("---");

  delay(3000);
}
