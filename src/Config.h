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
