#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int16.h>

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = 8, TXPin = 9;
static const uint32_t GPSBaud = 9600;
// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

TinyGPSCustom pdop(gps, "GNGLL", 1); // $GPGSA sentence, 15th element
TinyGPSCustom hdop(gps, "GNGLL", 3); // $GPGSA sentence, 16th element

//these pins can not be changed 2/3 are special pins
int encoderPin1 = 2;
int encoderPin2 = 3;
int encoderPin3 = 21;
int encoderPin4 = 20;

volatile int lastEncoded1 = 0;
//volatile long encoderValue1 = 0;
volatile int lastEncoded2 = 0;
//volatile long encoderValue2 = 0;

long lastencoderValue1 = 0;
long lastencoderValue2 = 0;

int lastMSB1 = 0;
int lastLSB1 = 0;
int lastMSB2 = 0;
int lastLSB2 = 0;

// Minumum and maximum values for 16-bit integers
const int encoder_minimum = -32768;
const int encoder_maximum = 32767;

// Handles startup and shutdown of ROS
ros::NodeHandle nh;
std_msgs::Int16 encoderValue1;
std_msgs::Int16 encoderValue2;

void updateEncoder1(){
  int MSB1 = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB1 = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB1 << 1) |LSB1; //converting the 2 pin value to single number
  int sum  = (lastEncoded1 << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011){
    if ( encoderValue1.data == encoder_maximum) encoderValue1.data = encoder_minimum;
    else encoderValue1.data ++;
  }
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000){
    if ( encoderValue1.data == encoder_minimum) encoderValue1.data = encoder_maximum;
    else encoderValue1.data --;
  }

  lastEncoded1 = encoded; //store this value for next time
}
void updateEncoder2(){
  int MSB2 = digitalRead(encoderPin3); //MSB = most significant bit
  int LSB2 = digitalRead(encoderPin4); //LSB = least significant bit

  int encoded = (MSB2 << 1) |LSB2; //converting the 2 pin value to single number
  int sum  = (lastEncoded2 << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011){
    if ( encoderValue2.data == encoder_maximum) encoderValue2.data = encoder_minimum;
    else encoderValue2.data ++;
  }
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000){
    if ( encoderValue2.data == encoder_minimum) encoderValue2.data = encoder_maximum;
    else encoderValue2.data --;
  }

  lastEncoded2 = encoded; //store this value for next time
}

// Keep track of the number of wheel ticks
ros::Publisher frontPub("front_ticks", &updateEncoder1);
ros::Publisher backPub("back_ticks", &updateEncoder2);

 
void setup() {
  //Serial.begin (115200);
  ss.begin(GPSBaud);
  
  pinMode(encoderPin1, INPUT); 
  pinMode(encoderPin2, INPUT);
  pinMode(encoderPin3, INPUT); 
  pinMode(encoderPin4, INPUT);

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin3, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin4, HIGH); //turn pullup resistor on

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
  attachInterrupt(0, updateEncoder1, CHANGE); 
  attachInterrupt(1, updateEncoder1, CHANGE);
  attachInterrupt(2, updateEncoder2, CHANGE); 
  attachInterrupt(3, updateEncoder2, CHANGE);

  nh.initNode();
  nh.advertise(frontPub);
  nh.advertise(backPub);

}

void loop(){ 
  //Do stuff here

  //Serial.print(encoderValue1);
  //Serial.print(", ");
  //Serial.println(encoderValue2);
  //Serial.print(F(" LAT=")); Serial.print(atof(pdop.value())/100,7);
  //Serial.print(F("\tLON=")); Serial.println(atof(hdop.value())/100,7);
  //delay(100); //just here to slow down the output, and show it will work  even during a delay
  while (ss.available() > 0)
    gps.encode(ss.read());
  frontPub.publish( &encoderValue1 );
  backPub.publish( &encoderValue2 );
  nh.spinOnce();
  delay(10);
}
