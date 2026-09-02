#!/usr/bin/env python3
"""
Sends a wheel command and polls R (RPM) repeatedly to watch PID
convergence over time. Usage:

    python3 pid_convergence_test.py <leftSpeed> <rightSpeed> [turns] [port]

Example - right wheel only, 30% power, 20 samples:
    python3 pid_convergence_test.py 0 0.3 20 /dev/ttyUSB0
"""
import serial
import sys
import time

def main():
    if len(sys.argv) < 3:
        print(__doc__)
        sys.exit(1)

    left = sys.argv[1]
    right = sys.argv[2]
    turns = int(sys.argv[3]) if len(sys.argv) > 3 else 20
    port = sys.argv[4] if len(sys.argv) > 4 else "/dev/ttyUSB0"

    s = serial.Serial(port, 115200, timeout=1)
    time.sleep(1)

    command = f"{left} {right}\n"
    print(f"--- send {command.strip()} ---")
    s.write(command.encode())
    time.sleep(0.3)

    for i in range(turns):
        s.write(b"R\n")
        time.sleep(0.15)
        r = s.readline().decode(errors="replace").rstrip()
        print(f"t={i*1.15:5.2f}s  {r}")
        time.sleep(1.0)

    print("--- stop ---")
    s.write(b"0 0\n")
    time.sleep(0.3)
    s.close()

if __name__ == "__main__":
    main()
