#include "SerialCommandSource.h"

void SerialCommandSource::begin() {
  Serial.println("READY");
  Serial.println("Format: leftSpeed rightSpeed");
  Serial.println("Example: 100 100");
}

void SerialCommandSource::poll() {
  while (Serial.available()) {

    char c = Serial.read();

    if (c == '\n') {

      input.trim();

      int space = input.indexOf(' ');

      if (space > 0) {
        cmdL = input.substring(0, space).toFloat();
        cmdR = input.substring(space + 1).toFloat();

        cmdL = constrain(cmdL, -1.0, 1.0);
        cmdR = constrain(cmdR, -1.0, 1.0);

        Serial.print("CMD L=");
        Serial.print(cmdL);
        Serial.print(" R=");
        Serial.println(cmdR);
      }

      input = "";
    }
    else {
      input += c;
    }
  }
}

float SerialCommandSource::getLeftCommand() const { return cmdL; }
float SerialCommandSource::getRightCommand() const { return cmdR; }
