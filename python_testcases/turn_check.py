import serial
import time

PORT = "/dev/ttyUSB1"
BAUD = 115200

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)
ser.reset_input_buffer()

def send(left, right):
    ser.write(f"{left} {right}\n".encode())

def drain(seconds):
    t0 = time.time()
    lines = []
    while time.time() - t0 < seconds:
        line = ser.readline().decode(errors="ignore").strip()
        if line:
            print(line)
            lines.append(line)
    return lines

print("Forcing stop (0 0) and confirming ticks are frozen")
send(0, 0)
drain(2)

print("Sending turn command 0 0.5 from a guaranteed cold stop, for 6s")
send(0, 0.5)
lines = drain(6)

print("Sending stop")
send(0, 0)
lines += drain(2)

ser.close()
