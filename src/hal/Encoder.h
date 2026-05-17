#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

// Encoder reads a quadrature encoder and counts tick increments.
class Encoder {
  int pinA, pinB;
  volatile long ticks = 0;

public:
  Encoder(int a, int b);

  // Configure encoder pins and attach an interrupt handler.
  void begin(void (*isr)());

  // Handle a change on channel A and update the tick count.
  void handleA();

  // Read the current tick count safely from the main loop.
  long getTicks();
};

#endif
