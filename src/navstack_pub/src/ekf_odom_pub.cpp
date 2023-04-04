// Include various libraries
#include "ros/ros.h"
#include "std_msgs/Int16.h"
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseStamped.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>
#include <cmath>
 
// Create odometry data publishers
ros::Publisher odom_data_pub;
ros::Publisher odom_data_pub_quat;
nav_msgs::Odometry odomNew;
nav_msgs::Odometry odomOld;
 
// Initial pose
const double initialX = 0.0;
const double initialY = 0.0;
const double initialTheta = 0.00000000001;
const double PI = 3.141592;

const double FTICKS_PER_METER = -32*PI*0.655/(62*2400);
const double BTICKS_PER_METER = -16*PI*0.655/(62*2400);
const double STICKS_PER_DEGREE = -21*2*PI/(26*2400);
const double Lfb = 1.045; // distance b/w two wheels

// Distance both wheels have traveled and angle of steering
double distanceF = 0;
double distanceB = 0;
double angleS = 0;
 
// Flag to see if initial pose has been received
bool initialPoseRecieved = false;
 
using namespace std;
 
// Get initial_2d message from either Rviz clicks or a manual pose publisher
void set_initial_2d(const geometry_msgs::PoseStamped &rvizClick) {
 
  odomOld.pose.pose.position.x = rvizClick.pose.position.x;
  odomOld.pose.pose.position.y = rvizClick.pose.position.y;
  odomOld.pose.pose.orientation.z = rvizClick.pose.orientation.z;
  initialPoseRecieved = true;
}
 
// Calculate the distance the front wheel has traveled since the last cycle
void Calc_Front(const std_msgs::Int16& frontCount) {
 
  static int lastCountF = 0;
  if(frontCount.data != 0 && lastCountF != 0) {
         
    int frontTicks = (frontCount.data - lastCountF);
 
    if (frontTicks > 10000) {
      frontTicks = frontTicks-65535;
    }
    else if (frontTicks < -10000) {
      frontTicks = 65535+frontTicks;
    }
    else{}
    distanceF = frontTicks*FTICKS_PER_METER;
  }
  lastCountF = frontCount.data;
}

// Calculate the distance the back wheel has traveled since the last cycle
void Calc_Back(const std_msgs::Int16& backCount) {
 
  static int lastCountB = 0;
  if(backCount.data != 0 && lastCountB != 0) {
         
    int backTicks = (backCount.data - lastCountB);
 
    if (backTicks > 10000) {
      backTicks =  backTicks-65535;
    }
    else if (backTicks < -10000) {
      backTicks = 65535+backTicks;
    }
    else{}
    distanceB = backTicks*BTICKS_PER_METER;
  }
  lastCountB = backCount.data;
}
 
// Calculate the steering angle
void Calc_Steer(const std_msgs::Int16& steerCount) {
   
  static int lastCountS = 0;
  if(steerCount.data != 0 && lastCountS != 0) {
 
    int steerTicks = steerCount.data;
     
    angleS = steerTicks*STICKS_PER_DEGREE;
  }
  lastCountS = steerCount.data;
}
 
// Publish a nav_msgs::Odometry message in quaternion format
void publish_quat() {
 
  tf2::Quaternion q;
         
  q.setRPY(0, 0, odomNew.pose.pose.orientation.z);
 
  nav_msgs::Odometry quatOdom;
  quatOdom.header.stamp = odomNew.header.stamp;
  quatOdom.header.frame_id = "odom";
  quatOdom.child_frame_id = "base_link";
  quatOdom.pose.pose.position.x = odomNew.pose.pose.position.x;
  quatOdom.pose.pose.position.y = odomNew.pose.pose.position.y;
  quatOdom.pose.pose.position.z = odomNew.pose.pose.position.z;
  quatOdom.pose.pose.orientation.x = q.x();
  quatOdom.pose.pose.orientation.y = q.y();
  quatOdom.pose.pose.orientation.z = q.z();
  quatOdom.pose.pose.orientation.w = q.w();
  quatOdom.twist.twist.linear.x = odomNew.twist.twist.linear.x;
  quatOdom.twist.twist.linear.y = odomNew.twist.twist.linear.y;
  quatOdom.twist.twist.linear.z = odomNew.twist.twist.linear.z;
  quatOdom.twist.twist.angular.x = odomNew.twist.twist.angular.x;
  quatOdom.twist.twist.angular.y = odomNew.twist.twist.angular.y;
  quatOdom.twist.twist.angular.z = odomNew.twist.twist.angular.z;
 
  for(int i = 0; i<36; i++) {
    if(i == 0 || i == 7 || i == 14) {
      quatOdom.pose.covariance[i] = .01;
     }
     else if (i == 21 || i == 28 || i== 35) {
       quatOdom.pose.covariance[i] += 0.1;
     }
     else {
       quatOdom.pose.covariance[i] = 0;
     }
  }
 
  odom_data_pub_quat.publish(quatOdom);
}
 
// Update odometry information
void update_odom() {
 
  // Calculate the new pose (x, y, and theta)
  odomNew.pose.pose.position.x = odomOld.pose.pose.position.x + cos(odomOld.pose.pose.orientation.z)*distanceB;
  odomNew.pose.pose.position.y = odomOld.pose.pose.position.y + sin(odomOld.pose.pose.orientation.z)*distanceB;
  odomNew.pose.pose.orientation.z = tan(angleS)*distanceB/(Lfb*cos(angleS)) + odomOld.pose.pose.orientation.z;
 
  // Prevent lockup from a single bad cycle
  if (isnan(odomNew.pose.pose.position.x) || isnan(odomNew.pose.pose.position.y)
     || isnan(odomNew.pose.pose.position.z)) {
    odomNew.pose.pose.position.x = odomOld.pose.pose.position.x;
    odomNew.pose.pose.position.y = odomOld.pose.pose.position.y;
    odomNew.pose.pose.orientation.z = odomOld.pose.pose.orientation.z;
  }
 
  // Compute the velocity
  odomNew.header.stamp = ros::Time::now();
  odomNew.twist.twist.linear.x = distanceB/(odomNew.header.stamp.toSec() - odomOld.header.stamp.toSec());
  odomNew.twist.twist.angular.z = (tan(angleS)*distanceB/(Lfb*cos(angleS)))/(odomNew.header.stamp.toSec() - odomOld.header.stamp.toSec());
 
  // Save the pose data for the next cycle
  odomOld.pose.pose.position.x = odomNew.pose.pose.position.x;
  odomOld.pose.pose.position.y = odomNew.pose.pose.position.y;
  odomOld.pose.pose.orientation.z = odomNew.pose.pose.orientation.z;
  odomOld.header.stamp = odomNew.header.stamp;
 
  // Publish the odometry message
  odom_data_pub.publish(odomNew);
}
 
int main(int argc, char **argv) {
   
  // Set the data fields of the odometry message
  odomNew.header.frame_id = "odom";
  odomNew.pose.pose.position.z = 0;
  odomNew.pose.pose.orientation.x = 0;
  odomNew.pose.pose.orientation.y = 0;
  odomNew.twist.twist.linear.x = 0;
  odomNew.twist.twist.linear.y = 0;
  odomNew.twist.twist.linear.z = 0;
  odomNew.twist.twist.angular.x = 0;
  odomNew.twist.twist.angular.y = 0;
  odomNew.twist.twist.angular.z = 0;
  odomOld.pose.pose.position.x = initialX;
  odomOld.pose.pose.position.y = initialY;
  odomOld.pose.pose.orientation.z = initialTheta;
 
  // Launch ROS and create a node
  ros::init(argc, argv, "ekf_odom_pub");
  ros::NodeHandle node;
 
  // Subscribe to ROS topics
  ros::Subscriber subForFrontCounts = node.subscribe("front_ticks", 100, Calc_Front, ros::TransportHints().tcpNoDelay());
  ros::Subscriber subForBackCounts = node.subscribe("back_ticks", 100, Calc_Back, ros::TransportHints().tcpNoDelay());
  ros::Subscriber subForSteerCounts = node.subscribe("steer_ticks", 100, Calc_Steer, ros::TransportHints().tcpNoDelay());
  ros::Subscriber subInitialPose = node.subscribe("initial_2d", 1, set_initial_2d);
 
  // Publisher of simple odom message where orientation.z is an euler angle
  odom_data_pub = node.advertise<nav_msgs::Odometry>("odom_data_euler", 100);
 
  // Publisher of full odom message where orientation is quaternion
  odom_data_pub_quat = node.advertise<nav_msgs::Odometry>("odom_data_quat", 100);
 
  ros::Rate loop_rate(30); 
     
  while(ros::ok()) {
     
    if(initialPoseRecieved) {
      update_odom();
      publish_quat();
    }
    if(!initialPoseRecieved){
      odomOld.pose.pose.position.x = 0;
      odomOld.pose.pose.position.y = 0;
      odomOld.pose.pose.orientation.z = 0;
      initialPoseRecieved = true;
    }
    ros::spinOnce();
    loop_rate.sleep();
  }
 
  return 0;
}
