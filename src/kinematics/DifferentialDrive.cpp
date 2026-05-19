#include "DifferentialDrive.h"

DifferentialDrive::DifferentialDrive(MotorController& l, MotorController& r)
  : left(l), right(r) {}

// Convert chassis linear/angular velocity into individual wheel commands.
// This simple differential-drive model assumes the angular input is already
// scaled for the track width or the expected wheel velocity differential.
void DifferentialDrive::setVelocity(float linear, float angular, float dt) {

  // Convert normalized input → motor scale
  float maxPWM = 255.0;

  float leftTarget  = (linear - angular) * maxPWM;
  float rightTarget = (linear + angular) * maxPWM;

  // Clamp for safety
  leftTarget  = constrain(leftTarget,  -maxPWM, maxPWM);
  rightTarget = constrain(rightTarget, -maxPWM, maxPWM);

  left.setOpenLoop(leftTarget);
  right.setOpenLoop(rightTarget);
}