import serial
import time
import csv
import matplotlib.pyplot as plt

# ---------------- CONFIG ----------------
PORT = "/dev/ttyUSB0"
BAUD = 115200
DURATION = 4
CSV_FILE = "encoder_log.csv"

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)

ser.reset_input_buffer()

# ---------------- SEND COMMAND ----------------
def send_cmd(left, right):
    ser.write(f"{left} {right}\n".encode())

# ---------------- PARSE ENCODER LINE ----------------
def parse_line(line):
    """
    Supports:
    1) 123,456
    2) L:123 R:456
    """

    try:
        line = line.strip()

        # Format 1: 123,456
        if "," in line and "L" not in line:
            l, r = line.split(",")
            return int(l), int(r)

        # Format 2: L:123 R:456
        if "L:" in line and "R:" in line:
            line = line.replace("L:", "").replace("R:", "")
            parts = line.split()
            return int(parts[0]), int(parts[1])

    except:
        return None

    return None

# ---------------- RUN TEST + LOG ----------------
def run_and_log():

    print("Starting rotation test + logging...")

    start_time = time.time()

    data = []

    # rotate robot
    send_cmd(-50, 50)

    with open(CSV_FILE, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["time", "left", "right"])

        while time.time() - start_time < DURATION:

            line = ser.readline().decode(errors="ignore").strip()

            if not line:
                continue

            print("RAW:", line)

            result = parse_line(line)

            if result is None:
                continue

            l, r = result
            t = time.time() - start_time

            writer.writerow([t, l, r])
            data.append((t, l, r))

    # stop robot
    send_cmd(0, 0)

    print("Logging complete.")
    return data

# ---------------- PLOT ----------------
def plot(data):

    if len(data) == 0:
        print("No data to plot! Check encoder output.")
        return

    t = [d[0] for d in data]
    l = [d[1] for d in data]
    r = [d[2] for d in data]

    plt.figure()

    plt.plot(t, l, label="Left Encoder")
    plt.plot(t, r, label="Right Encoder")

    plt.xlabel("Time (s)")
    plt.ylabel("Ticks")
    plt.title("Encoder Rotation Test (Log + Plot)")
    plt.legend()
    plt.grid()

    plt.show()

# ---------------- MAIN ----------------
if __name__ == "__main__":

    data = run_and_log()

    print("Plotting results...")

    plot(data)

    ser.close()