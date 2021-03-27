#!/usr/bin/env python

import rospy
from geometry_msgs.msg import Twist
import serial
import struct
import Jetson.GPIO as GPIO

# Parameter to scale the motors speed.
global lin_gain
global ang_gain

# Connection to the RPI Pico
pico = serial.Serial(
    port="/dev/ttyTHS1",
    baudrate=115200,
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE
)


def callback(velMsg):
	# Log the requested value (DEBUG)
	rospy.loginfo("topic value:" +str(velMsg.linear.x)+str(velMsg.angular.z))
	
	# Compute the desired speed for each motor, using the gains
	vdx = (velMsg.linear.x)*lin_gain		# Using the linear velocity
	vsx = vdx
	vdx += velMsg.angular.z*ang_gain		# And the angular velocity
	vsx -= velMsg.angular.z*ang_gain
	vsx = min(100, max(-100, vsx))			#Constrain the values between 100% and -100%
	vdx = min(100, max(-100, vdx))
	
	rospy.loginfo("requested: "+str(vdx)+" "+str(vsx)) #Log the computed value
	
	pico.write('s'.encode()) 				# 's' is the start character for code running in the PICO
	pico.write(struct.pack("b",vsx))		# Sends left and right speed, encoding in 1 byte signed
	pico.write(struct.pack("b",-vdx))	

if __name__ == '__main__':
	# Use GPIO 7 to signal the PICO to start driving the motors
	GPIO.setmode(GPIO.BOARD)
	GPIO.setup(7, GPIO.OUT, initial=GPIO.HIGH)

	rospy.init_node('motcontrol', anonymous=False)		# ROS node init
	lin_gain = rospy.get_param('~lin_gain', 90)			# parameters init
	ang_gain = rospy.get_param('~ang_gain', 50)
	rospy.loginfo("%s is %s", rospy.resolve_name('~lin_gain'), lin_gain)	# log parameters value, using the resolve_name trick
	rospy.loginfo("%s is %s", rospy.resolve_name('~ang_gain'), ang_gain) 
	rospy.Subscriber("cmd_vel", Twist, callback)		# subscribe to topic
	rospy.loginfo("started") 
	rospy.spin()

