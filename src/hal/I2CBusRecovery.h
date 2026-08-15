#pragma once

// Recovers a wedged I2C bus (e.g. a slave left holding SDA low after a
// transaction was interrupted mid-flight) by manually toggling SCL to
// force the slave to release the line, then issuing a STOP condition.
// Must be called before Wire.begin(), using the same pins.
void recoverI2CBus(int sdaPin, int sclPin);
