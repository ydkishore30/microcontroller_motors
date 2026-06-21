#pragma once

// Hardware values that must be adjusted for your specific robot before
// flashing. Nothing in this file should require changes to any other
// source file.
//
// WiFi/agent credentials are NOT here - they are entered at runtime
// over Serial and stored in flash (see RuntimeConfig.h), so they never
// end up in source code or git history.

// --- INA226 current sensor ---
// Match the shunt resistor actually fitted on your current sensor board.
#define SHUNT_RESISTOR_OHMS 0.1f
#define MAX_EXPECTED_CURRENT_A 3.0f

// --- Closed-loop velocity control ---
// Measured open-loop speed at full command was ~1050-1300 ticks/sec
// (see python_testcases/rotate_test.py results). A command of 1.0 maps
// to this many ticks/sec as the PID target speed.
#define MAX_WHEEL_SPEED_TICKS_PER_SEC 1300.0f
