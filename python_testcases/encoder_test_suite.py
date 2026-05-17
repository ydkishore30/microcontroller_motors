from robot import Robot
import time

robot = Robot()

# ---------------------------
# Helper: read encoder line
# ---------------------------
def read_sample(n=20, delay=0.05):
    data = []
    for _ in range(n):
        line = robot.read()
        if line:
            data.append(line)
        time.sleep(delay)
    return data


# ---------------------------
# TEST 1: Noise Test (motor OFF)
# ---------------------------
def test_noise():
    print("\n=== TEST 1: Noise Test ===")

    robot.send(0, 0)
    time.sleep(2)

    start = robot.read()
    time.sleep(2)
    end = robot.read()

    print("Start:", start)
    print("End  :", end)

    print("→ Check: should NOT change much\n")


# ---------------------------
# TEST 2: Tick consistency (manual rotation)
# ---------------------------
def test_tick_consistency():
    print("\n=== TEST 2: Tick Consistency ===")

    print("Rotate wheel slowly by hand...")

    samples = read_sample(30, 0.1)

    for s in samples:
        print(s)

    print("→ Check: smooth incremental change (no jumps)\n")


# ---------------------------
# TEST 3: Forward symmetry
# ---------------------------
def test_forward_symmetry():
    print("\n=== TEST 3: Forward Symmetry ===")

    robot.send(100, 100)
    time.sleep(3)
    robot.send(0, 0)

    samples = read_sample(20)

    for s in samples:
        print(s)

    print("→ Check: L ≈ R\n")


# ---------------------------
# TEST 4: Reverse symmetry
# ---------------------------
def test_reverse_symmetry():
    print("\n=== TEST 4: Reverse Symmetry ===")

    robot.send(-100, -100)
    time.sleep(3)
    robot.send(0, 0)

    samples = read_sample(20)

    for s in samples:
        print(s)

    print("→ Check: L ≈ R (negative trend)\n")


# ---------------------------
# TEST 5: Rotation test
# ---------------------------
def test_rotation():
    print("\n=== TEST 5: Rotation Test ===")

    robot.send(-100, 100)
    time.sleep(2)
    robot.send(0, 0)

    samples = read_sample(20)

    for s in samples:
        print(s)

    print("→ Check: L and R opposite signs\n")


# ---------------------------
# TEST RUNNER
# ---------------------------
if __name__ == "__main__":

    test_noise()
    time.sleep(2)

    test_tick_consistency()
    time.sleep(2)

    test_forward_symmetry()
    time.sleep(2)

    test_reverse_symmetry()
    time.sleep(2)

    test_rotation()

    robot.close()