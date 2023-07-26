#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/String.h>

#define DAC1 25
#define DIR 33
#define dirPin 26
#define stepPin 27

int cmdVelFwd = 0.0;
int cmdVelRot = 0.0;


ros::NodeHandle nh;

void cmdVelCallBack(const geometry_msgs::Twist& twist) {
  cmdVelFwd = round( (double)twist.linear.x ); //testing
  cmdVelRot = round( (double)twist.angular.z ); //testing

  if(cmdVelFwd > 100) { cmdVelFwd = 100; }
  else if (cmdVelFwd < -100) { cmdVelFwd = -100; }

  if(cmdVelRot > 255) { cmdVelRot = 255; }
  else if (cmdVelRot <-255) { cmdVelRot = -255; }
}

ros::Subscriber<geometry_msgs::Twist> subCmdVel("cmd_vel", cmdVelCallBack);

geometry_msgs::Twist twist_msg;
ros::Publisher check("check", &twist_msg);

void setup() {
  nh.initNode();
  nh.subscribe(subCmdVel);
  nh.advertise(check);
  pinMode(DIR,OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  digitalWrite(dirPin, LOW); // CW direction
  pinMode(DIR,LOW); // Forward direction
}

void loop() {
  nh.spinOnce();
  if(cmdVelFwd > 0){
    digitalWrite(dirPin,LOW);
  } else if (cmdVelFwd < 0) {
    digitalWrite(dirPin, HIGH);
  } 
  for (int i = 0; i < cmdVelRot; i++) {
    // These four lines result in 1 step:
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1200);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1200);
  } 
  dacWrite(DAC1, cmdVelFwd);
  twist_msg.linear.x = cmdVelFwd;
  twist_msg.angular.z = cmdVelRot;
  check.publish( &twist_msg );
  nh.spinOnce();
  delay(10);
}
