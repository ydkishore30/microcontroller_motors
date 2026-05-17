import serial
import time

class Robot:
    def __init__(self, port="/dev/ttyUSB1", baud=115200):
        self.ser = serial.Serial(port, baud, timeout=1)
        time.sleep(2)

    def send(self, linear, angular):
        cmd = f"{linear} {angular}\n"
        self.ser.write(cmd.encode())

    def read(self):
        return self.ser.readline().decode().strip()

    def stop(self):
        self.send(0, 0)

    def close(self):
        self.ser.close()