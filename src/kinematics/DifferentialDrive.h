#ifndef DIFFERENTIAL_DRIVE_H
#define DIFFERENTIAL_DRIVE_H

#include "../control/MotorController.h"

// DifferentialDrive converts high-level chassis commands into left and right
// wheel targets for a two-wheel differential drive robot.
class DifferentialDrive {
  MotorController& left;
  MotorController& right;

public:
  DifferentialDrive(MotorController& l, MotorController& r);

  // Set desired linear and angular velocity for the robot chassis.
  // linear: forward/backward speed
  // angular: turning rate
  // dt: elapsed time since the previous update (seconds)
  void setVelocity(float linear, float angular, float dt);
};

#endif
