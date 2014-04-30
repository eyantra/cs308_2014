import serial
import time

msleep = lambda x: time.sleep(x/1000.0)
ser = serial.Serial(3)

def sendChar(c, t):
    print "Writing ", c
    ser.write(c)
    #x = ser.read()
    #print "Received ",x
    msleep(t)
'''
while True:
    z = raw_input("Enter: ")
    ser.write(z)
    x = ser.read()
    print x
    time.sleep(1)
'''