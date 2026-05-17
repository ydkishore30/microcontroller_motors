import serial
import time
import csv

# ---------------- CONFIG ----------------
PORT = "/dev/ttyUSB1"
BAUD = 115200

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)
ser.reset_input_buffer()

CSV_FILE = "motor_test_log.csv"

# ---------------- SEND COMMAND ----------------
def send(left, right):
    cmd = f"{left} {right}\n"
    ser.write(cmd.encode())

# ---------------- READ SERIAL ----------------
def read_serial():
    line = ser.readline().decode(errors="ignore").strip()
    if line:
        print("RAW:", line)
    return line

# ---------------- TEST CASE ----------------
def run_test(name, commands, delay=2):

    print("\n==============================")
    print("TEST:", name)
    print("==============================")

    with open(CSV_FILE, "a", newline="") as f:
        writer = csv.writer(f)

        writer.writerow([name, "time", "left_cmd", "right_cmd", "raw"])

        start = time.time()

        for cmd in commands:
            left, right = cmd

            print(f"Sending → L:{left} R:{right}")
            send(left, right)

            t0 = time.time()

            while time.time() - t0 < delay:
                raw = read_serial()
                writer.writerow([name, time.time() - start, left, right, raw])
                time.sleep(0.05)

        send(0, 0)
        print("STOP")

# ---------------- TEST SET ----------------
tests = [
    ("LEFT ONLY", [(80, 0)]),
    ("RIGHT ONLY", [(0, 80)]),
    ("FORWARD", [(80, 80)]),
    ("REVERSE", [(-80, -80)]),
    ("TURN LEFT", [(-80, 80)]),
    ("TURN RIGHT", [(80, -80)]),
]

# ---------------- MAIN ----------------
if __name__ == "__main__":

    print("Starting Motor Test Suite...")

    for name, cmds in tests:
        run_test(name, cmds, delay=3)
        time.sleep(2)

    send(0, 0)
    ser.close()

    print("Testing complete. Data saved to CSV.")