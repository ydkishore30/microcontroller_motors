import re
import time

import serial

PORT = "/dev/ttyUSB1"
BAUD = 115200
TICKS_PER_REV = 1260

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)
ser.reset_input_buffer()


def send(left, right):
    ser.write(f"{left} {right}\n".encode())


def drain_timed(seconds):
    t0 = time.time()
    samples = []
    pattern = re.compile(r"R ticks=(-?\d+)")
    while time.time() - t0 < seconds:
        line = ser.readline().decode(errors="ignore").strip()
        if not line:
            continue
        print(line)
        m = pattern.search(line)
        if m:
            samples.append((time.time(), int(m.group(1))))
    return samples


print("Stopping")
send(0, 0)
drain_timed(2)

print("Sending OPEN-LOOP full speed (0 1) for 15s")
send(0, 1)
samples = drain_timed(15)

print("Stopping")
send(0, 0)
drain_timed(2)
ser.close()

if len(samples) >= 2:
    window_start_time = samples[-1][0] - 5.0
    window = [s for s in samples if s[0] >= window_start_time]
    t_start, ticks_start = window[0]
    t_end, ticks_end = window[-1]
    dt = t_end - t_start
    dticks = ticks_end - ticks_start
    ticks_per_sec = dticks / dt if dt > 0 else 0
    rpm = (ticks_per_sec / TICKS_PER_REV) * 60.0
    print(f"\nSteady-state window: dt={dt:.2f}s, dticks={dticks}")
    print(f"Measured open-loop: {ticks_per_sec:.1f} ticks/sec = {rpm:.1f} RPM")
else:
    print("Not enough samples captured")
