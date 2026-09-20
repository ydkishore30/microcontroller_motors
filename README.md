# microcontroller_motors

ESP32 firmware for a differential-drive robot: closed-loop PID velocity
control over two motors with quadrature encoders, a BNO055 IMU, an
INA226 current sensor, and a plain-Serial command protocol compatible
with a ros2_control custom hardware interface.

## Hardware

**Motor driver:** Cytron MDD3A, in `PWM_PWM` mode (two PWM inputs per
motor, no separate direction pin).

| Signal | Pin |
|---|---|
| Left motor PWM A / PWM B | GPIO 27 / GPIO 14 |
| Right motor PWM A / PWM B | GPIO 25 / GPIO 26 |
| Left encoder A / B | GPIO 33 / GPIO 32 |
| Right encoder A / B | GPIO 35 / GPIO 34 |
| I2C SDA / SCL (BNO055 + INA226) | GPIO 21 / GPIO 22 |

Encoder resolution: 1260 ticks/rev (`ENCODER_TICKS_PER_REV` in
`src/Config.h`). Pin numbers are `#define`d at the top of
`src/main.cpp`.

**IMU:** BNO055, 9-axis with onboard sensor fusion, run in NDOF mode
(`src/hal/BNO055.cpp`). Default I2C address `0x28`.

**Current sensor:** INA226 (`src/hal/INA226.cpp`). Optional - if not
wired, its `begin()` fails independently at boot and the rest of the
firmware keeps running.

## Build & flash

Uses PlatformIO. The default environment is `esp32dev_serial`:

```bash
pio run -e esp32dev_serial -t upload --upload-port /dev/ttyUSB0
```

Other environments (see `platformio.ini`):
- `i2c_scanner_test` - scans the full I2C address range, independent of
  any sensor driver. Useful for isolating "bus dead" vs "wrong
  address"/"bad sensor".
- `esp32dev_microros` / `esp32dev_microros_serial` - micro-ROS builds
  (not currently wired into `main.cpp`'s `setup()`/`loop()`).

## Serial protocol

115200 baud. One command per line (`\n`-terminated). Velocity commands
are fire-and-forget; `E`/`R`/`I`/`O`/`C` are synchronous queries that
each return exactly one reply line.

| Command | Reply | Meaning |
|---|---|---|
| `<left> <right>` (e.g. `0.3 0.3`) | *(none)* | Set wheel speeds, each in `[-1, 1]`. Positive = forward. `0 0` stops immediately. |
| `E` | `E <leftTicks> <rightTicks>` | Raw encoder tick counts. |
| `R` | `R <leftRPM> <rightRPM>` | Estimated wheel speed in RPM. |
| `I` | `I <ax> <ay> <az> <gx> <gy> <gz>` | Accelerometer (g) and gyroscope (deg/s), BNO055 axes. |
| `O` | `O <heading> <roll> <pitch>` | Fused orientation in degrees (BNO055 NDOF fusion). `0 0 0` on a sensor without onboard fusion. |
| `C` | `C <leftCmd> <rightCmd>` | Last velocity command received (manual-testing convenience). |

Example, using Python:

```python
import serial, time
s = serial.Serial("/dev/ttyUSB0", 115200, timeout=1)
time.sleep(2)  # wait for boot

s.write(b"0.3 0.3\n")   # drive forward
time.sleep(2)
s.write(b"R\n"); print(s.readline())   # wheel RPM
s.write(b"O\n"); print(s.readline())   # orientation
s.write(b"0 0\n")        # stop
```

### Notes

- `E`/`R`/`I`/`O` are synchronous: nothing else may print to `Serial`
  between receiving a query and sending its reply, or a line-timeout
  read on the other end will desync. This is why `SerialTelemetryPublisher`
  is not used alongside `SerialCommandSource`, and why sensors that fail
  `begin()` are never polled afterwards (an unthrottled failing I2C
  transaction logs to `Serial` on every attempt, which would corrupt the
  protocol).
- `python_testcases/pid_convergence_test.py` sends a command and polls
  `R` repeatedly to watch PID convergence:
  ```bash
  python3 python_testcases/pid_convergence_test.py <left> <right> [turns] [port]
  ```

## Velocity control

`USE_CLOSED_LOOP_PID` in `src/Config.h` selects open-loop (command
applied directly as motor power) or closed-loop (PID tracks a
ticks/sec target derived from the command, using encoder feedback).
Currently closed-loop, tuned to `kp=0.0008, ki=0.0004` (see
`src/main.cpp`).
