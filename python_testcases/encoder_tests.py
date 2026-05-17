import time

class EncoderTests:

    # ---------------------------
    # TEST 1: Noise test (no motion)
    # ---------------------------
    def noise_test(self, robot):
        print("TEST 1: Noise Test")

        start = None
        values = []

        for _ in range(20):
            line = robot.read()
            values.append(line)

        print("Samples:", values)
        return values


    # ---------------------------
    # TEST 2: Forward motion
    # ---------------------------
    def forward_test(self, robot):
        print("TEST 2: Forward")

        robot.send(100, 100)
        time.sleep(3)
        robot.send(0, 0)

        data = []
        for _ in range(20):
            data.append(robot.read())

        return data


    # ---------------------------
    # TEST 3: Reverse motion
    # ---------------------------
    def reverse_test(self, robot):
        print("TEST 3: Reverse")

        robot.send(-100, -100)
        time.sleep(3)
        robot.send(0, 0)

        data = []
        for _ in range(20):
            data.append(robot.read())

        return data


    # ---------------------------
    # TEST 4: Rotation test
    # ---------------------------
    def rotation_test(self, robot):
        print("TEST 4: Rotation")

        robot.send(-100, 100)
        time.sleep(2)
        robot.send(0, 0)

        data = []
        for _ in range(20):
            data.append(robot.read())

        return data