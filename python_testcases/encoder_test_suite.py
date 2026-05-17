import time
import csv
import matplotlib.pyplot as plt
from robot import Robot
from tests import RobotTests

# ---------------- INIT ----------------
robot = Robot()
tests = RobotTests()

# ---------------- TEST LIST ----------------
test_list = [
    ("forward", tests.test_forward),
    ("backward", tests.test_backward),
    ("turn_left", tests.test_turn_left),
    ("turn_right", tests.test_turn_right),
    ("ramp", tests.test_ramp)
]

# ---------------- FILES ----------------
log_file = open("logs.txt", "w")
csv_file = open("encoder_log.csv", "w", newline="")
writer = csv.writer(csv_file)

writer.writerow(["test", "time", "left", "right"])

# ---------------- PARSER ----------------
def parse(line):
    """
    Supports:
    - 123,456
    - L:123 R:456
    """

    try:
        line = line.strip()

        if "," in line and "L" not in line:
            l, r = line.split(",")
            return int(l), int(r)

        if "L:" in line and "R:" in line:
            line = line.replace("L:", "").replace("R:", "")
            parts = line.split()
            return int(parts[0]), int(parts[1])

    except:
        return None

    return None

# ---------------- DATA COLLECT ----------------
def collect_data(test_name, duration=2.0):

    start = time.time()
    data = []

    while time.time() - start < duration:

        line = robot.read()

        if not line:
            continue

        parsed = parse(line)

        if parsed is None:
            continue

        l, r = parsed
        t = time.time() - start

        writer.writerow([test_name, t, l, r])
        data.append((t, l, r))

    return data

# ---------------- PLOT FUNCTION ----------------
def plot_all(data_map):

    plt.figure()

    for name, data in data_map.items():

        if len(data) == 0:
            continue

        t = [d[0] for d in data]
        l = [d[1] for d in data]
        r = [d[2] for d in data]

        plt.plot(t, l, label=f"{name} L")
        plt.plot(t, r, linestyle="--", label=f"{name} R")

    plt.xlabel("Time (s)")
    plt.ylabel("Encoder Ticks")
    plt.title("Robot Encoder Test Results")
    plt.legend()
    plt.grid()

    plt.show()

# ---------------- MAIN LOOP ----------------
if __name__ == "__main__":

    results = {}

    for name, test in test_list:

        print("\n======================")
        print("Running:", name)
        print("======================")

        # 1. run motion
        result = test(robot)

        # 2. collect encoder data
        data = collect_data(name, 2.0)
        results[name] = data

        # 3. log summary
        log = f"{name} -> {result} | samples={len(data)}"
        print(log)

        log_file.write(log + "\n")

        time.sleep(1)

    # ---------------- STOP ROBOT ----------------
    robot.stop()
    robot.close()

    log_file.close()
    csv_file.close()

    # ---------------- PLOT ----------------
    print("\nPlotting results...")
    plot_all(results)