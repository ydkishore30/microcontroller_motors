#pragma once

// Hardware values that must be adjusted for your specific robot before
// flashing. Nothing in this file should require changes to any other
// source file.
//
// WiFi/agent credentials are NOT here - they are entered at runtime
// over Serial and stored in flash (see RuntimeConfig.h), so they never
// end up in source code or git history.

// --- I2C sensors (BNO055 + INA226) ---
// Wired to the ESP32's SDA/SCL pins (see I2C_SDA/I2C_SCL in main.cpp).
// MPU6500 physically connected - MPU6050.cpp already accepts its
// WHO_AM_I (0x70) alongside the MPU6050's (0x68). If INA226 isn't
// actually wired, its own begin() check fails independently and just
// logs "Current sensor init failed" without blocking the IMU.
#define ENABLE_I2C_SENSORS 1

// Match the shunt resistor actually fitted on your current sensor board.
#define SHUNT_RESISTOR_OHMS 0.1f
#define MAX_EXPECTED_CURRENT_A 3.0f

// --- Velocity control mode ---
// 0 = open loop (command applied directly as motor power, ramped for a
//     smooth start). 1 = closed loop (PID tracks a ticks/sec target
//     derived from the command, using encoder feedback). Disabled (not
//     removed) for now - the PID/MotorController::update() code below
//     is untouched and still fully usable, just not currently called.
#define USE_CLOSED_LOOP_PID 1

// Encoder resolution, used to convert raw ticks to revolutions/RPM.
#define ENCODER_TICKS_PER_REV 1260.0f

// --- Closed-loop velocity control ---
// Motor is rated 100 RPM; measured open-loop ceiling was ~94 RPM (see
// python_testcases/openloop_rpm_check.py). Target operating speed is
// set to 80 RPM, below both, so the PID always has headroom to correct
// without riding the output-saturation boundary - pushing the target
// too close to the true max previously caused it to do that and
// vibrate (see git history). 80 RPM = 80/60 rev/sec * 1260 ticks/rev =
// 1680 ticks/sec. A command of 1.0 maps to this many ticks/sec as the
// PID target speed.
#define MAX_WHEEL_SPEED_TICKS_PER_SEC 1680.0f

// Maximum allowed change in target speed per second. The PID target is
// ramped at this rate instead of jumping instantly to the commanded
// speed, so acceleration is smooth regardless of PID tuning. At this
// rate, going from 0 to full speed takes ~2.1s. Raised from 300 so the
// ramped target climbs past encoder tick-quantization noise faster,
// shrinking the brief reverse-direction jerk seen at the start of a
// command (noisy low-tick-count speed readings can momentarily exceed
// a still-tiny target early in the ramp).
#define MAX_ACCEL_TICKS_PER_SEC2 600.0f
