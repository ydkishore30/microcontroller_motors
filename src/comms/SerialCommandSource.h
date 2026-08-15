#pragma once
#include <Arduino.h>
#include "ICommandSource.h"

// Parses "leftSpeed rightSpeed" lines from Serial, e.g. "100 100".
class SerialCommandSource : public ICommandSource {
public:
  void begin() override;
  void poll() override;

  float getLeftCommand() const override;
  float getRightCommand() const override;

private:
  String input = "";
  float cmdL = 0.0f;
  float cmdR = 0.0f;
};
