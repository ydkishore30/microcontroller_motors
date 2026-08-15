#include "MicroRosSerialTransport.h"
#include <micro_ros_platformio.h>

MicroRosSerialTransport::MicroRosSerialTransport(Stream& stream) : stream(stream) {}

void MicroRosSerialTransport::begin() {
  set_microros_serial_transports(stream);
}
