import csv
import os
import re
import time

import matplotlib.pyplot as plt
import serial

PORT = "/dev/ttyUSB1"
BAUD = 115200
TARGET_TICKS_PER_SEC = 1680.0

OUTPUT_DIR = os.path.join(os.path.dirname(__file__), "plots")
os.makedirs(OUTPUT_DIR, exist_ok=True)

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


print("Forcing stop")
send(0, 0)
drain_timed(2)

t_send = time.time()
print("Sending full speed (0 1) - step response starts now")
send(0, 1)
samples = drain_timed(20)

print("Stopping")
send(0, 0)
drain_timed(2)
ser.close()

times = []
rates = []
for i in range(1, len(samples)):
    t0, ticks0 = samples[i - 1]
    t1, ticks1 = samples[i]
    dt = t1 - t0
    if dt <= 0:
        continue
    rate = (ticks1 - ticks0) / dt
    times.append(t1 - t_send)
    rates.append(rate)

plt.figure(figsize=(10, 6))
plt.plot(times, rates, marker="o", markersize=3, label="Measured speed (ticks/sec)")
plt.axhline(TARGET_TICKS_PER_SEC, color="red", linestyle="--", label=f"Target ({TARGET_TICKS_PER_SEC:.0f} ticks/sec = 80 RPM)")
plt.xlabel("Time since command (s)")
plt.ylabel("Wheel speed (ticks/sec)")
plt.title("Step Response: Right Wheel, Full Command (0 1)")
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
stamp = time.strftime("%Y%m%d_%H%M%S")
out_path = os.path.join(OUTPUT_DIR, f"step_response_{stamp}.png")
plt.savefig(out_path, dpi=120)
print(f"Saved plot to {out_path}")

csv_path = os.path.join(OUTPUT_DIR, f"step_response_{stamp}.csv")
with open(csv_path, "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(["elapsed_s", "rate_ticks_per_sec"])
    for t, r in zip(times, rates):
        writer.writerow([f"{t:.3f}", f"{r:.1f}"])
print(f"Saved raw values to {csv_path}")
