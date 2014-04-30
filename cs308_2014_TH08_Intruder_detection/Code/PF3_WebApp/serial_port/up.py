import sys
import serial 
import time

ser = serial.Serial(2)
ser.write('8')
ser.close