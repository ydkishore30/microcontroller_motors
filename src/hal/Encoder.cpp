#include "Encoder.h"

Encoder::Encoder(int a, int b) : pinA(a), pinB(b) {}

void Encoder::begin(void (*isr)()) {
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinA), isr, CHANGE);
}

void Encoder::handleA() {
  // Quadrature decoding using the relationship between pin A and pin B.
  // If both inputs match, the encoder is moving in one direction; otherwise,
  // it is moving in the opposite direction.
  if (digitalRead(pinA) == digitalRead(pinB))
    ticks++;
  else
    ticks--;
}

long Encoder::getTicks() {
  // Safely read the tick count without interruption while copying.
  noInterrupts();
  long t = -ticks;
  interrupts();
  return t;
}
