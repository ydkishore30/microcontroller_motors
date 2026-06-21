#include "I2CBusRecovery.h"
#include <Arduino.h>

void recoverI2CBus(int sdaPin, int sclPin) {
  pinMode(sdaPin, INPUT_PULLUP);
  pinMode(sclPin, OUTPUT);
  digitalWrite(sclPin, HIGH);

  // If a slave is holding SDA low, pulse SCL up to 9 times - each clock
  // pulse lets a stuck slave shift out one more bit until it releases SDA.
  for (int i = 0; i < 9 && digitalRead(sdaPin) == LOW; i++) {
    digitalWrite(sclPin, LOW);
    delayMicroseconds(5);
    digitalWrite(sclPin, HIGH);
    delayMicroseconds(5);
  }

  // Issue a STOP condition (SDA rising while SCL is high) so any device
  // mid-transaction returns to an idle state.
  pinMode(sdaPin, OUTPUT);
  digitalWrite(sdaPin, LOW);
  delayMicroseconds(5);
  digitalWrite(sclPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(sdaPin, HIGH);
  delayMicroseconds(5);
}
