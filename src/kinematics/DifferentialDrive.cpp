#include "DifferentialDrive.h"

DifferentialDrive::DifferentialDrive(MotorController& l, MotorController& r)
  : left(l), right(r) {}

// Convert chassis linear/angular velocity into individual wheel commands.
// This simple differential-drive model assumes the angular input is already
// scaled for the track width or the expected wheel velocity differential.
void DifferentialDrive::setVelocity(float linear, float angular, float dt) {
  float leftTarget  = linear - angular;
  float rightTarget = linear + angular;

  // For open-loop control, write values directly to each wheel controller.
  // To enable feedback control, uncomment the update calls and tune the PID.
  // left.update(leftTarget, dt);
  // right.update(rightTarget, dt);

  left.setOpenLoop(leftTarget);
  right.setOpenLoop(rightTarget);
}
