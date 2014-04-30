import serial 
import time
import sys

ser = serial.Serial(12)
n = 4
arr = [[[0,0] for y in range(n)] for x in range(n)]
while 1 :
    a = ser.read(1)
    print a
    if a == 'B':
        break
    elif a == 'A':
        x = ser.read(1)
        y = ser.read(1)
        left = ser.read(1)
        top = ser.read(1)
        right = ser.read(1)
        bottom = ser.read(1)

        print x,y,left,top,right,bottom
        
        X = int(x)
        Y = int(y)
        Left = int(left)
        Top = int(top)
        Right = int(right)
        Bottom = int(bottom)

        arr[X][Y][0] = Right
        arr[X][Y][1] = Bottom

f = open('map.txt', 'r+')

for x in range(n):
    s1 = ""
    s2 = ""
    for y in range(n):
        s1 += "*"
        if arr[x][y][0] == 1:
            s1 += "--"
        else:
            s1 += "  "
        if arr[x][y][1] == 1:
            s2 += "|"
        else:
            s2 += " "
        s2 += "  "
    s1 += "\n"+s2+"\n"
    f.write(s1)

f.close()

count = 0
while 1:
    x = raw_input()
    y = raw_input()
    ser.write(x)
    ser.write(y)
    count = count+1
    
ser.close()
