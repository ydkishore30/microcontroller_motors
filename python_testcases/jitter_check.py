import re
import time

import serial

PORT = "/dev/ttyUSB1"
BAUD = 115200

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)
ser.reset_input_buffer()


def send(left, right):
    ser.write(f"{left} {right}\n".encode())


def drain_timed(seconds):
    t0 = time.time()
    samples = []
    pattern = re.compile(r"R ticks: (-?\d+)")
    while time.time() - t0 < seconds:
        line = ser.readline().decode(errors="ignore").strip()
        if not line:
            continue
        m = pattern.search(line)
        if m:
            samples.append((time.time(), int(m.group(1))))
    return samples


print("Stopping")
send(0, 0)
drain_timed(2)

print("Sending full speed (0 1) for 15s")
send(0, 1)
samples = drain_timed(30)

print("Stopping")
send(0, 0)
drain_timed(2)
ser.close()

print(f"\n{len(samples)} samples captured")
print("Last 25 samples (timestamp delta, tick delta, instantaneous ticks/sec):")
for i in range(max(1, len(samples) - 25), len(samples)):
    t0, ticks0 = samples[i - 1]
    t1, ticks1 = samples[i]
    dt = t1 - t0
    dticks = ticks1 - ticks0
    rate = dticks / dt if dt > 0 else 0
    print(f"  dt={dt:.3f}s  dticks={dticks:5d}  rate={rate:8.1f} ticks/sec")
