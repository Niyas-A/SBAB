#!/usr/bin/env python
# license removed for brevity
import rospy
from geometry_msgs.msg import Twist
import serial
import csv
from time import sleep


arduino_port = "/dev/ttyACM0" #serial port of Arduino
baud = 9600 #arduino uno runs at 9600 baud
fileName="data10march.csv" #name of the CSV file generated
ser = serial.Serial(arduino_port, baud)
print("Connected to Arduino port:" + arduino_port)
sleep(.1)
A=-62*2400*7/(32*22*0.655)
B=-62*2400*7/(16*22*0.655)
C=26*2400/(21*360)

def encoder_pub():
  #global msg
  msg = Twist()
  pub = rospy.Publisher('encoder', Twist, queue_size=10)
  rospy.init_node('encoderPub', anonymous=True)
  rate = rospy.Rate(10) # 10hz
  while not rospy.is_shutdown():
    ser.flushInput()
    getData=ser.readline()
    data = getData.decode()
    readings = data.split(",")
    print(readings)
    if len(readings)==3:
      msg.linear.x = int(readings[0])/A
      msg.linear.y = int(readings[1])/B
      msg.angular.z = int(readings[2])/C
      #rospy.loginfo(hello_str)
      pub.publish(msg)
    rate.sleep()

if __name__ == '__main__':
    try:
        encoder_pub()
    except rospy.ROSInterruptException:
        pass
