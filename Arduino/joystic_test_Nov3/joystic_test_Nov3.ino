/*int x_key = 34;                                               
int y_key = 35;                                               
float x_pos;
float y_pos;

void setup ( ) {
Serial.begin (9600) ;
pinMode (x_key, INPUT) ;                     
pinMode (y_key, INPUT) ;                      
}

void loop ( ) {
x_pos = analogRead (x_key) ;  
x_pos = 2*(x_pos-2047)/4095;
y_pos = analogRead (y_key) ;
y_pos = 2*(y_pos-2047)/4095;                      
Serial.print("X:");
Serial.print(x_pos);
Serial.print(" Y:");
Serial.println(y_pos);
}
*/
#include <ros.h>
#include <geometry_msgs/Twist.h>

#define x_key 34
#define y_key 35
#define MaxFwd 100
#define MaxRot 5


float cmdVelFwd = 0.0;
float cmdVelRot = 0.0;

ros::NodeHandle nh;

geometry_msgs::Twist twist_msg;
ros::Publisher joypub("cmd_vel", &twist_msg);

void setup() {
  nh.initNode();
  nh.advertise(joypub);
  pinMode(x_key, INPUT);
  pinMode(y_key, INPUT);
}

void loop() {
  cmdVelFwd = analogRead (x_key) ;  
  cmdVelFwd = 2*(cmdVelFwd-2047)/4095;
  cmdVelRot = analogRead (y_key) ;
  cmdVelRot = 2*(cmdVelRot-2047)/4095;
  if (abs(cmdVelFwd)<0.1){
    cmdVelFwd=0;                     
  }
  if (cmdVelFwd>1){
    cmdVelFwd=1;                     
  }
  if (cmdVelFwd<-1){
    cmdVelFwd=-1;                     
  }
  if (abs(cmdVelRot)<0.12){
    cmdVelRot=0;                     
  }
  if (cmdVelRot>1){
    cmdVelRot=1;                     
  }
  if (cmdVelRot<-1){
    cmdVelRot=-1;                     
  }
  //Serial.print("X:");
  //Serial.print(x_pos);
  //Serial.print(" Y:");
  //Serial.println(y_pos);
  twist_msg.linear.x = cmdVelFwd*MaxFwd;
  twist_msg.angular.z = cmdVelRot*MaxRot;
  joypub.publish( &twist_msg );
  nh.spinOnce();
  delay(10);
}
