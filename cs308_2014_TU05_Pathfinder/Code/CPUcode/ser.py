import serial

ser=serial.Serial("/dev/ttyUSB0",115200)
ser.close()
ser.open()
#print ser.read(10)
#while ser.read(1) == "S" :
	#continue
