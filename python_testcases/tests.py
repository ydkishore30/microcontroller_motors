import time

class RobotTests:

    def test_forward(self, robot):
        print("TEST: Forward")
        robot.send(50, 0)
        time.sleep(3)
        robot.stop()
        return robot.read()

    def test_backward(self, robot):
        print("TEST: Backward")
        robot.send(-50, 0)
        time.sleep(3)
        robot.stop()
        return robot.read()

    def test_turn_left(self, robot):
        print("TEST: Turn Left")
        robot.send(0, 40)
        time.sleep(3)
        robot.stop()
        return robot.read()

    def test_turn_right(self, robot):
        print("TEST: Turn Right")
        robot.send(0, -40)
        time.sleep(3)
        robot.stop()
        return robot.read()

    def test_ramp(self, robot):
        print("TEST: Ramp Test")

        for v in range(0, 80, 10):
            robot.send(v, 0)
            time.sleep(0.5)

        robot.stop()
        return robot.read()