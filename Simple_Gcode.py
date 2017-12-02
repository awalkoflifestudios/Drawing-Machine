import serial
from datetime import datetime

start_time = datetime.now()
MoveInterval = 200 # ms between move commands so it doesn't backup

arduino = serial.Serial('COM4', 115200, timeout=.1)

def millis():
   dt = datetime.now() - start_time
   ms = (dt.days * 24 * 60 * 60 + dt.seconds) * 1000 + dt.microseconds / 1000.0
   return ms

last_time_moved = millis()

for yposition in range ( 0, 500, 10 ):
    while millis() - last_time_moved < MoveInterval:
          False # python hates empty blocks
    cmd = "G01 Y" + str(yposition)+ " F1000;"
    print(cmd)
    arduino.write(cmd + "\r\n")
    last_time_moved = millis()

