#define DAC1 3
#define DIR 2
#define dirPin 4
#define stepPin 5

int cmdVelFwd = 0.0;
int cmdVelRot = 0.0;

void setup() {
  Serial.begin(115200);
  pinMode(DIR,OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(DAC1, OUTPUT);
  digitalWrite(dirPin, LOW); // CW direction
  pinMode(DIR,LOW); // Forward direction
  Serial.println("Setup done..");
}

void loop() {
  if(Serial.available()){
    Serial.println("Serial Available..");
    cmdVelFwd = Serial.parseInt();
    cmdVelRot = Serial.parseInt();
    Serial.print("Fwd value: ");
    Serial.println(cmdVelFwd);
    Serial.print("Rot value: ");
    Serial.println(cmdVelRot);
  //nh.spinOnce();
  if(cmdVelFwd > 0){ //Forward
    digitalWrite(DIR,LOW);
  } else if (cmdVelFwd < 0) { //backward OR brake
    digitalWrite(DIR, HIGH);
  } 
  if(cmdVelRot > 0){ //CW
    digitalWrite(dirPin,LOW);
  } else if (cmdVelRot < 0) { //ACW
    digitalWrite(dirPin, HIGH);
  }
  for (int i = 0; i < abs(cmdVelRot); i++) {
    // These four lines result in 1 step:
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1200);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1200);
  } 
  analogWrite(DAC1, abs(cmdVelFwd));
}
}
