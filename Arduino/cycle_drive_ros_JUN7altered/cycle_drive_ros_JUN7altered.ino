#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <Servo.h>

Servo ESC;
#define escPin 10

int cmdVelFwd = 0.0;

ros::NodeHandle nh;

void cmdVelCallBack(const geometry_msgs::Twist& twist) {
  cmdVelFwd = round( (double)twist.linear.x ); //testing

  if(cmdVelFwd > 100) { cmdVelFwd = 100; }
  else if (cmdVelFwd < 0) { cmdVelFwd = 0; }
}

ros::Subscriber<geometry_msgs::Twist> subCmdVel("cmd_vel", cmdVelCallBack);

geometry_msgs::Twist twist_msg;
ros::Publisher check("check", &twist_msg);

void setup() {
  nh.initNode();
  nh.subscribe(subCmdVel);
  nh.advertise(check);
  ESC.attach(escPin);
}

void loop() {
  nh.spinOnce();
  if(cmdVelFwd>=0 && cmdVelFwd<=100){
    cmdVelFwd = map(cmdVelFwd, 0, 100, 90, 150);
    ESC.write(cmdVelFwd);  
  }
  twist_msg.linear.x = cmdVelFwd;
  check.publish( &twist_msg );
  nh.spinOnce();
  delay(10);
}
