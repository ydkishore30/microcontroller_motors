import time
import serial

PORT = "/dev/ttyUSB0"
BAUD = 115200

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)

ser.reset_input_buffer()

# ---------------- SEND COMMAND ----------------
def send(left, right):
    cmd = f"{left} {right}\n"
    print("SENT:", cmd.strip())
    ser.write(cmd.encode())

# ---------------- READ ----------------
def read_encoder():
    line = ser.readline().decode(errors="ignore").strip()
    if not line:
        return None

    print("RAW:", line)
    return line

# ---------------- TEST FUNCTION ----------------
def test_wheel(name, left, right, duration=3):

    print("\n======================")
    print("TEST:", name)
    print("======================")

    send(left, right)

    start = time.time()

    while time.time() - start < duration:
        read_encoder()
        time.sleep(0.05)

    send(0, 0)
    time.sleep(1)

# ---------------- MAIN ----------------
if __name__ == "__main__":

    # LEFT wheel only
    test_wheel("LEFT ONLY", 80, 0)

    # RIGHT wheel only
    test_wheel("RIGHT ONLY", 0, 80)

    # BOTH wheels
    test_wheel("BOTH", 80, 80)

    ser.close()