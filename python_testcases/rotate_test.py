import re
import sys
import time

import serial

PORT = "/dev/ttyUSB1"
BAUD = 115200
SPEED = 0.2

TICK_RE = re.compile(r"L ticks=(-?\d+).*R ticks=(-?\d+)")


def parse_ticks(line):
    match = TICK_RE.search(line)
    if not match:
        return None
    return int(match.group(1)), int(match.group(2))


def run(target_ticks, timeout_s=15):
    ser = serial.Serial(PORT, BAUD, timeout=1)
    time.sleep(2)
    ser.reset_input_buffer()

    # Find a starting tick baseline before commanding any motion.
    start_left = start_right = None
    deadline = time.time() + 3
    while (start_left is None or start_right is None) and time.time() < deadline:
        line = ser.readline().decode(errors="ignore").strip()
        parsed = parse_ticks(line)
        if parsed:
            start_left, start_right = parsed

    if start_left is None:
        print("Could not read a baseline tick count - check connection/firmware")
        ser.close()
        return

    print(f"Baseline: L={start_left} R={start_right}")
    print(f"Driving both wheels at speed={SPEED} until {target_ticks} ticks each...")

    ser.write(f"{SPEED} {SPEED}\n".encode())

    left_done = right_done = False
    deadline = time.time() + timeout_s

    while time.time() < deadline:
        line = ser.readline().decode(errors="ignore").strip()
        parsed = parse_ticks(line)
        if not parsed:
            continue

        left, right = parsed
        left_delta = abs(left - start_left)
        right_delta = abs(right - start_right)

        if left_delta >= target_ticks:
            left_done = True
        if right_delta >= target_ticks:
            right_done = True

        if left_done and right_done:
            break

        # Stop whichever wheel has already reached target, keep the other going.
        l_speed = 0 if left_done else SPEED
        r_speed = 0 if right_done else SPEED
        ser.write(f"{l_speed} {r_speed}\n".encode())

    ser.write(b"0 0\n")
    time.sleep(0.3)

    final_left = final_right = None
    deadline = time.time() + 2
    while time.time() < deadline:
        line = ser.readline().decode(errors="ignore").strip()
        parsed = parse_ticks(line)
        if parsed:
            final_left, final_right = parsed
            break

    ser.close()

    if final_left is None:
        print("Could not read final tick count")
        return

    left_delta = abs(final_left - start_left)
    right_delta = abs(final_right - start_right)

    print(f"RESULT: left={left_delta} ticks, right={right_delta} ticks "
          f"(target was {target_ticks})")


if __name__ == "__main__":
    target = int(sys.argv[1]) if len(sys.argv) > 1 else 1260
    run(target)
