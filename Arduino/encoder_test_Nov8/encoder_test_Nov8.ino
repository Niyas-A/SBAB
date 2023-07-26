// Encoder output to Arduino Interrupt pin. Tracks the tick count.
#define ENC_IN_LEFT_A 2
 
// Other encoder output to Arduino to keep track of wheel direction
// Tracks the direction of rotation.
#define ENC_IN_LEFT_B 4
 
// True = Forward; False = Reverse
boolean Direction_left = true;
 
// Minumum and maximum values for 16-bit integers
const int encoder_minimum = -32768;
const int encoder_maximum = 32767;
 
// Keep track of the number of wheel ticks
volatile int left_wheel_tick_count = 0;
 
// One-second interval for measurements
int interval = 1000;
long previousMillis = 0;
long currentMillis = 0;
 
void setup() {
 
  // Open the serial port at 9600 bps
  Serial.begin(9600); 
 
  // Set pin states of the encoder
  pinMode(ENC_IN_LEFT_A , INPUT_PULLUP);
  pinMode(ENC_IN_LEFT_B , INPUT);
 
  // Every time the pin goes high, this is a tick
  attachInterrupt(digitalPinToInterrupt(ENC_IN_LEFT_A), left_wheel_tick, RISING);
}
 
void loop() {
 
  // Record the time
  currentMillis = millis();
 
  // If one second has passed, print the number of ticks
  if (currentMillis - previousMillis > interval) {
     
    previousMillis = currentMillis;
 
    Serial.println("Number of Ticks: ");
    Serial.println(left_wheel_tick_count);
    Serial.println();
  }
}
 

// Increment the number of ticks
void left_wheel_tick() {
   
  // Read the value for the encoder for the left wheel
  int val = digitalRead(ENC_IN_LEFT_B);
 
  if(val == LOW) {
    Direction_left = true; // Reverse
  }
  else {
    Direction_left = false; // Forward
  }
   
  if (Direction_left) {
    if (left_wheel_tick_count == encoder_maximum) {
      left_wheel_tick_count = encoder_minimum;
    }
    else {
      left_wheel_tick_count++;  
    }  
  }
  else {
    if (left_wheel_tick_count == encoder_minimum) {
      left_wheel_tick_count = encoder_maximum;
    }
    else {
      left_wheel_tick_count--;  
    }   
  }
}
