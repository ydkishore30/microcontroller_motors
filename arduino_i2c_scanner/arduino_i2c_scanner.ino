#include <Wire.h>

// Standalone I2C scanner for Arduino Uno/Nano/Mega - separate from the
// ESP32 project entirely. Uno/Nano use fixed I2C pins: SDA=A4, SCL=A5.
// Wire it up: sensor VCC->5V (or 3.3V pin if your Arduino has one and the
// sensor needs it), GND->GND, SDA->A4, SCL->A5.

void setup() {
  Wire.begin();
  Serial.begin(9600);
  while (!Serial) { }
  Serial.println("I2C Scanner ready");
}

void loop() {
  int found = 0;

  Serial.println("Scanning...");

  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Device found at 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
      found++;
    } else if (error == 4) {
      Serial.print("Unknown error at 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
    }
  }

  if (found == 0) {
    Serial.println("No I2C devices found");
  } else {
    Serial.print("Done. Devices found: ");
    Serial.println(found);
  }

  Serial.println("---");
  delay(3000);
}
