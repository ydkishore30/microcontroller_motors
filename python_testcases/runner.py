from robot import Robot
from tests import RobotTests
import time
import csv

robot = Robot()
tests = RobotTests()

test_list = [
    ("forward", tests.test_forward),
    ("backward", tests.test_backward),
    ("turn_left", tests.test_turn_left),
    ("turn_right", tests.test_turn_right),
    ("ramp", tests.test_ramp)
]

log_file = open("logs.txt", "w")
csv_file = open("encoder_log.csv", "w", newline="")
writer = csv.writer(csv_file)

# CSV header
writer.writerow(["test", "timestamp", "raw_data"])

def collect_data(duration=2.0):
    """Collect encoder data during motion"""
    start = time.time()
    data = []

    while time.time() - start < duration:
        line = robot.read()
        if line:
            data.append((time.time(), line))
            writer.writerow(["", time.time(), line])
    return data


for name, test in test_list:
    print("\n======================")
    print("Running:", name)
    print("======================")

    # 1. run motion
    result = test(robot)

    # 2. collect encoder data after motion
    data = collect_data(2.0)

    # 3. log summary
    log = f"{name} -> {result} | samples={len(data)}"
    print(log)

    log_file.write(log + "\n")

    time.sleep(1)

# Stop robot safely
robot.stop()
robot.close()

log_file.close()
csv_file.close()