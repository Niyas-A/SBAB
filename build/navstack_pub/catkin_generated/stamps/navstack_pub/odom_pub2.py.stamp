#!/usr/bin/env python

import math
import rospy
import tf
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Point, Pose, Quaternion, Twist, Vector3
from std_msgs.msg import Int16
from math import sin, cos, pi,tan

rospy.init_node('odom_pub2')

odom_pub = rospy.Publisher("odom", Odometry, queue_size=50)
odom_broadcaster = tf.TransformBroadcaster()

x = 0.0
y = 0.0
th = 0.0

vx = 0.1
vy = -0.1
vth = 0.1

PI = 3.141592;
FTICKS_PER_METER = -32*PI*0.655/(62*2400);
BTICKS_PER_METER = -16*PI*0.655/(62*2400);
STICKS_PER_DEGREE = -21*2*PI/(26*2400);
Lfb = 1.045; # distance b/w two wheels

# Distance both wheels have traveled and angle of steering
distanceF = 0.0;
distanceB = 0.0;
angleS = 0.0;
#global lastCountF,lastCountB,lastCountS
lastCountF = 0
lastCountB = 0
lastCountS = 0
frontTicks = 0
backTicks = 0
steerTicks = 0 
current_time = rospy.Time.now()
last_time = rospy.Time.now()

r = rospy.Rate(100)

def getFTicks(data):
  global frontTicks,lastCountF
  frontTicks = data.data

def getBTicks(data):
  global backTicks,lastCountB
  backTicks = data.data

def getSTicks(data): 
  global steerTicks,lastCountS
  steerTicks = data.data

if __name__ == '__main__':
  print "Running"
  #rospy.init_node('odom_pub2')
  while not rospy.is_shutdown():
    rospy.Subscriber('front_ticks', Int16, getFTicks)
    rospy.Subscriber('back_ticks', Int16, getBTicks)
    rospy.Subscriber('steer_ticks', Int16, getSTicks)    
    current_time = rospy.Time.now()    

    if(backTicks != 0 and lastCountB != 0):         
      BackTicks = (backTicks - lastCountB)
      if (BackTicks > 50000): 
        print "backticks>50000"
        print(BackTicks)
        BackTicks = BackTicks-65535
      elif (BackTicks < -50000):
        print "backticks<-50000"
        print(BackTicks)
        BackTicks = 65535+BackTicks
      distanceB = BackTicks*BTICKS_PER_METER
    lastCountB = backTicks
    if(frontTicks != 0 and lastCountF != 0):  
      FrontTicks = (frontTicks - lastCountF)
      if (FrontTicks > 30000):
        FrontTicks = FrontTicks-65535
      elif (FrontTicks < -30000):
        FrontTicks = 65535+FrontTicks
      distanceF = FrontTicks*FTICKS_PER_METER
    lastCountF = frontTicks
  
    if(steerTicks != 0 and lastCountS != 0):         
      angleS = steerTicks*STICKS_PER_DEGREE;
    lastCountS = steerTicks

    # compute odometry in a typical way given the velocities of the robot
    dt = (current_time - last_time).to_sec()
    delta_x = cos(th)*distanceB
    delta_y = sin(th)*distanceB
    delta_th = tan(angleS)*distanceB/(Lfb*cos(angleS))
    #print(distanceB)
    #print(angleS)
    distanceB = 0.0
    angleS = 0.0
    x += delta_x
    y += delta_y
    th += delta_th
    
    vx = delta_x/dt
    vy = delta_y/dt
    vth = delta_th/dt

    # since all odometry is 6DOF we'll need a quaternion created from yaw
    odom_quat = tf.transformations.quaternion_from_euler(0, 0, th)

    # first, we'll publish the transform over tf
    odom_broadcaster.sendTransform(
        (x, y, 0.),
        odom_quat,
        current_time,
        "base_link",
        "odom"
    )

    # next, we'll publish the odometry message over ROS
    odom = Odometry()
    odom.header.stamp = current_time
    odom.header.frame_id = "odom"

    # set the position
    odom.pose.pose = Pose(Point(x, y, 0.), Quaternion(*odom_quat))
    odom.pose.covariance = [0.01, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.01, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.01, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.01]
    # set the velocity
    odom.child_frame_id = "base_link"
    odom.twist.twist = Twist(Vector3(vx, vy, 0), Vector3(0, 0, vth))
    odom.twist.covariance = [0.01, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.01, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.01, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.01]
    # publish the message
    odom_pub.publish(odom)
    #print(x)
    #print(y)
    #print(th)
    last_time = current_time
    r.sleep()
    #rospy.spin()
