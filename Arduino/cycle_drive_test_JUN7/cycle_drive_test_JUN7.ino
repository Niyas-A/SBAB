#include <Servo.h>

Servo esc;

int escPin = 10;

void setup() {
  Serial.begin(9600);
  esc.attach(escPin);
}

void loop() {
  if (Serial.available()) {
    int value = Serial.parseInt(); // Read the integer value from the serial monitor
    Serial.read(); // Clear the newline character from the input buffer
    // Ensure the value is within the valid range of 0 to 180
    if (value >= 0 && value <= 180) {
      esc.write(value); // Set the motor speed based on the received value
      //esc.writeMicroseconds(1200);
      Serial.print("Setting motor speed to: ");
      Serial.println(value);
    } else {
      Serial.println("Invalid value. Enter a value between 0 and 180.");
    }
  }
}
