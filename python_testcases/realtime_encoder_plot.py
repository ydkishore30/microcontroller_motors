import serial
import time
import matplotlib.pyplot as plt
from collections import deque

# ---------------- CONFIG ----------------
PORT = "/dev/ttyUSB0"
BAUD = 115200

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)

# ---------------- BUFFER (sliding window) ----------------
MAX_POINTS = 200

t_data = deque(maxlen=MAX_POINTS)
l_data = deque(maxlen=MAX_POINTS)
r_data = deque(maxlen=MAX_POINTS)

start_time = time.time()

# ---------------- PLOT SETUP ----------------
plt.ion()
fig, ax = plt.subplots()

line1, = ax.plot([], [], label="Left Encoder")
line2, = ax.plot([], [], label="Right Encoder")

ax.set_title("Real-Time Encoder Rotation Graph")
ax.set_xlabel("Time (s)")
ax.set_ylabel("Ticks")
ax.legend()
ax.grid(True)

# ---------------- SEND COMMAND ----------------
def send_cmd(left, right):
    ser.write(f"{left} {right}\n".encode())

# ---------------- READ ENCODER ----------------
def read_encoder():
    try:
        line = ser.readline().decode().strip()
        if line:
            l, r = map(int, line.split(","))
            return l, r
    except:
        pass
    return None, None

# ---------------- START ROTATION ----------------
print("Starting rotation test...")

send_cmd(-100, 100)   # rotate robot

# ---------------- LIVE LOOP ----------------
while True:

    l, r = read_encoder()

    if l is not None:
        t = time.time() - start_time

        t_data.append(t)
        l_data.append(l)
        r_data.append(r)

        # update graph data
        line1.set_data(t_data, l_data)
        line2.set_data(t_data, r_data)

        ax.relim()
        ax.autoscale_view()

        plt.pause(0.01)

    # safety stop after some time (optional)
    if len(t_data) > 180:
        send_cmd(0, 0)
        print("Test finished")
        break

# stop motor
send_cmd(0, 0)
ser.close()
plt.ioff()
plt.show()