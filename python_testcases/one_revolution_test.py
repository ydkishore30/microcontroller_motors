import re
import time

import serial

PORT = "/dev/ttyUSB1"
BAUD = 115200
TICKS_PER_REV = 1260
SPEED_CMD = 0.3  # lower speed -> less overshoot from command/loop latency

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)
ser.reset_input_buffer()

pattern = re.compile(r"R ticks: (-?\d+)")


def send(left, right):
    ser.write(f"{left} {right}\n".encode())


def read_r_ticks():
    line = ser.readline().decode(errors="ignore").strip()
    if not line:
        return None
    m = pattern.search(line)
    return int(m.group(1)) if m else None


print("Forcing stop")
send(0, 0)
time.sleep(1)
ser.reset_input_buffer()

start_ticks = None
while start_ticks is None:
    start_ticks = read_r_ticks()

print(f"Start ticks: {start_ticks}. Driving right wheel at {SPEED_CMD} for one revolution ({TICKS_PER_REV} ticks)")
send(0, SPEED_CMD)

target = start_ticks + TICKS_PER_REV
last_ticks = start_ticks
while last_ticks - start_ticks < TICKS_PER_REV:
    t = read_r_ticks()
    if t is not None:
        last_ticks = t

send(0, 0)
print(f"Stop sent at {last_ticks} ticks (delta {last_ticks - start_ticks}, target {TICKS_PER_REV})")

# Let it settle and report final resting position
time.sleep(1.5)
ser.reset_input_buffer()
final_ticks = None
t0 = time.time()
while time.time() - t0 < 2:
    t = read_r_ticks()
    if t is not None:
        final_ticks = t

ser.close()

if final_ticks is not None:
    delta = final_ticks - start_ticks
    overshoot = delta - TICKS_PER_REV
    print(f"\nFinal settled ticks: {final_ticks}")
    print(f"Actual revolution delta: {delta} ticks (target {TICKS_PER_REV})")
    print(f"Overshoot: {overshoot} ticks ({100 * overshoot / TICKS_PER_REV:.1f}%)")
