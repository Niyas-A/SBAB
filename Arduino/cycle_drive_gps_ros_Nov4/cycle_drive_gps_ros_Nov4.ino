#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/String.h>
#include <Servo.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define DAC1 3
#define DIR 2
#define dirPin 4
#define stepPin 5
#define escPin 10
#define RXPin 8
#define TXPin 9
static const uint32_t GPSBaud = 9600;

Servo ESC;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
TinyGPSCustom pdop(gps, "GNGLL", 1); // $GPGSA sentence, 15th element
TinyGPSCustom hdop(gps, "GNGLL", 3); // $GPGSA sentence, 16th element

int cmdVelFwd = 0.0;
int cmdVelRot = 0.0;
float Lat = 0.0;
float Lon = 0.0;

ros::NodeHandle nh;

void cmdVelCallBack(const geometry_msgs::Twist& twist) {
  cmdVelFwd = round( (double)twist.linear.x ); //testing
  cmdVelRot = round( (double)twist.angular.z ); //testing

  if(cmdVelFwd > 150) { cmdVelFwd = 150; }
  else if (cmdVelFwd < -150) { cmdVelFwd = -150; }

  if(cmdVelRot > 150) { cmdVelRot = 150; }
  else if (cmdVelRot <-150) { cmdVelRot = -150; }
}

ros::Subscriber<geometry_msgs::Twist> subCmdVel("cmd_vel", cmdVelCallBack);

geometry_msgs::Twist twist_msg;
ros::Publisher check("check", &twist_msg);

void setup() {
  nh.initNode();
  nh.subscribe(subCmdVel);
  nh.advertise(check);
  ESC.attach(escPin,1000,2000);
  ss.begin(GPSBaud);
  pinMode(DIR,OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(DAC1, OUTPUT);
  digitalWrite(dirPin, HIGH); // CW direction
  pinMode(DIR,LOW); // Forward direction
}

void loop() {
  nh.spinOnce();
  Lat = atof(pdop.value())/100;
  Lon = atof(hdop.value())/100;
  while (ss.available() > 0)
    gps.encode(ss.read());
  if(cmdVelFwd > 0){ //Forward
    digitalWrite(DIR,LOW);
  } else if (cmdVelFwd < 0) { //backward OR brake
    digitalWrite(DIR, HIGH);
  } 
  if(cmdVelRot > 0){ //CW
    digitalWrite(dirPin,HIGH);
  } else if (cmdVelRot < 0) { //ACW
    digitalWrite(dirPin, LOW);
  }
  for (int i = 0; i < abs(cmdVelRot); i++) {
    // These four lines result in 1 step:
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1200);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1200);
  } 
  analogWrite(DAC1, abs(cmdVelFwd));
  if(abs(cmdVelFwd)<120){
    ESC.write(abs(cmdVelFwd));  
  }
  twist_msg.linear.x = cmdVelFwd;
  twist_msg.angular.z = cmdVelRot;
  check.publish( &twist_msg );
  nh.spinOnce();
  delay(10);
}
