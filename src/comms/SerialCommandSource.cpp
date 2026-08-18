#include "SerialCommandSource.h"
#include "../Config.h"

SerialCommandSource::SerialCommandSource(IEncoder& leftEncoder, IEncoder& rightEncoder)
  : leftEncoder(leftEncoder), rightEncoder(rightEncoder) {}

void SerialCommandSource::begin() {
  Serial.println("READY");
  Serial.println("Format: leftSpeed rightSpeed | E (encoder ticks) | R (RPM) | C (last command)");
  Serial.println("Example: 100 100");
}

void SerialCommandSource::poll() {
  updateVelocityEstimate();

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      input.trim();
      handleLine(input);
      input = "";
    } else if (c != '\r') {
      input += c;
    }
  }
}

void SerialCommandSource::handleLine(const String& line) {
  if (line == "E") {
    Serial.print("E ");
    Serial.print(leftEncoder.getTicks());
    Serial.print(" ");
    Serial.println(rightEncoder.getTicks());
    return;
  }

  if (line == "R") {
    Serial.print("R ");
    Serial.print(leftRPM, 2);
    Serial.print(" ");
    Serial.println(rightRPM, 2);
    return;
  }

  // "C" (Command) is a manual-testing convenience, not part of the
  // my_hardware.cpp protocol - it reports the last velocity command
  // actually received, since that command itself is silent by design.
  // Harmless alongside ros2_control: it never sends "C", so this never
  // fires and never produces unprompted output there.
  if (line == "C") {
    Serial.print("C ");
    Serial.print(cmdL, 2);
    Serial.print(" ");
    Serial.println(cmdR, 2);
    return;
  }

  // Velocity command "leftSpeed rightSpeed" - fire-and-forget, no reply
  // (matches my_hardware.cpp's write(), which doesn't read one back).
  int space = line.indexOf(' ');
  if (space > 0) {
    cmdL = constrain(line.substring(0, space).toFloat(), -1.0f, 1.0f);
    cmdR = constrain(line.substring(space + 1).toFloat(), -1.0f, 1.0f);
  }
}

void SerialCommandSource::updateVelocityEstimate() {
  unsigned long now = millis();
  unsigned long elapsedMs = now - lastVelocityUpdate;

  // Cap the update rate (~20Hz) so dt never gets so small that tick-count
  // quantization noise dominates the RPM estimate.
  if (elapsedMs < 50) return;

  float dt = elapsedMs / 1000.0f;
  long leftTicks = leftEncoder.getTicks();
  long rightTicks = rightEncoder.getTicks();

  leftRPM = ((leftTicks - prevLeftTicks) / ENCODER_TICKS_PER_REV) / dt * 60.0f;
  rightRPM = ((rightTicks - prevRightTicks) / ENCODER_TICKS_PER_REV) / dt * 60.0f;

  prevLeftTicks = leftTicks;
  prevRightTicks = rightTicks;
  lastVelocityUpdate = now;
}

float SerialCommandSource::getLeftCommand() const { return cmdL; }
float SerialCommandSource::getRightCommand() const { return cmdR; }
