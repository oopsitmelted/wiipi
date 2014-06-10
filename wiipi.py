#! /usr/bin/env python
import serial
import cwiid
from time import sleep

def clamp(n, minn, maxn):
    return max(min(maxn, n), minn)

def setmotors(ser, x, z):
	# x is the steering input, z is the speed. Both range from 100 to 154 with
	# 127 being nominal. 

	x = clamp(x, 100, 154)
	x = (x - 127) / 27.0
	
	z = clamp(z, 100, 154)
	z = (z - 100) / 54.0

	left_speed = int(127 * z)
	right_speed = int(127 * z)

	if x > 0:
		x = min(x, z)
		left_speed -= int(x * 127)
	elif x < 0:
		x = min(-x, z)
		right_speed -= int(x * 127)

	print(str(left_speed) + " " + str(right_speed) )
	ser.write("\x80")
	ser.write(chr(left_speed))
	ser.write("\x82")
	ser.write(chr(right_speed))

def main():

	# Open serial port
	try:
		print "Attempting to open serial port"
		ser = serial.Serial('/dev/rfcomm0', 115200, timeout = 0)
	except Exception, e:
		print "Error opening serial port" + str(e)
		exit()

	# Success
	print "Serial port opened successfully"

	wm = cwiid.Wiimote()
	wm.rpt_mode = cwiid.RPT_BTN | cwiid.RPT_ACC

	while True:

		setmotors(ser, wm.state['acc'][1], wm.state['acc'][2])
		sleep(0.1)

main()
