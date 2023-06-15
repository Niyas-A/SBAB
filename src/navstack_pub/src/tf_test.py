#!/usr/bin/env python

import rospy
import tf2_ros
import tf2_geometry_msgs
from std_msgs.msg import Int16
from geometry_msgs.msg import TransformStamped
from tf.transformations import quaternion_from_euler


# Create a TF broadcaster
tf_broadcaster = tf2_ros.StaticTransformBroadcaster()

def steer_ticks_callback(msg):
    # Callback function to handle steer_ticks messages
    # Adjust the yaw of the steering based on the steer_ticks value
    # Compute the new transform and publish it

    # Assuming the steering frame is "steering" and the target frame is "frame"
    # Modify the code below according to your specific frame names and initial XYZ values

    # Initial XYZ values of the transformation
    initial_x = 0.035
    initial_y = -0.100
    initial_z = 0.82

    # Get the steer_ticks value from the message
    steer_ticks = msg.data

    # Compute the desired yaw angle based on the steer_ticks value
    # Modify the calculation based on your specific requirements
    desired_yaw = -steer_ticks * 21*2*3.14/(2400*26)  # Example: Scaling steer_ticks by a factor of 0.01

    # Create a TransformStamped message
    transform = TransformStamped()
    transform.header.stamp = rospy.Time.now()
    transform.header.frame_id = "frame"
    transform.child_frame_id = "steering"
    transform.transform.translation.x = initial_x
    transform.transform.translation.y = initial_y
    transform.transform.translation.z = initial_z
    #transform.transform.rotation = quaternion_from_euler(0, 0, desired_yaw)
    # Convert the quaternion to a list
    quaternion = quaternion_from_euler(0, 0, desired_yaw)
    transform.transform.rotation.x = quaternion[0]
    transform.transform.rotation.y = quaternion[1]
    transform.transform.rotation.z = quaternion[2]
    transform.transform.rotation.w = quaternion[3]

    # Publish the transform
    tf_broadcaster.sendTransform(transform)

def main():
    rospy.init_node('dynamic_steering_transform')

    # Create a TF2 ROS TransformBroadcaster
    #tf_broadcaster = tf2_ros.TransformBroadcaster()

    # Subscribe to the steer_ticks topic
    rospy.Subscriber("/steer_ticks", Int16, steer_ticks_callback)

    rospy.spin()

if __name__ == '__main__':
    main()

