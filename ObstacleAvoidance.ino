/**
 * VT Astrobotics Obstacle Avoidance programming. Assumes 4 IR
 * sensors as input and 4 motors as possible outputs, each
 * located at the 4 corners of the vehicle.
 * Will turn left upon encountering rightward obstacles and
 * vise-versa.
 */

// @authors: Adam Barnes, Pranav
// @version: 0.0.1

/**
 * IRs 1-4
 * Pin 5 IR Sensor (Top Right)
 * Pin 6 IR Sensor (Top Left)
 * Pin 7 IR Sensor (Lower Right)
 * Pin 8 IR Sensor (Lower Left)
 */

#include "Servo.h"
Servo motorR1;
Servo motorR2;
Servo motorL1;
Servo motorL2;

// IR pins
const int IR1_pin =  5;    // Front
const int IR2_pin = 6;
const int IR3_pin = 7;    // Back
const int IR4_pin = 8;

// Motor pins
const int MotorL1Pin = 2;    // Left
const int MotorL2Pin = 3;
const int MotorR1Pin = 13;    // Right
const int MotorR2Pin = 0;

// Navigation Status
boolean isRightObst = false;
boolean isLeftObst = false;
boolean isFrontObst = false;
boolean isBackObst = false;

void setup() {
  Serial.begin(9600);
  Serial.println ("Starting");
  // IR
  pinMode(IR1_pin, INPUT);
  pinMode(IR2_pin, INPUT);
  pinMode(IR3_pin, INPUT);
  pinMode(IR4_pin, INPUT);
  // Motors
  motorR1.attach(MotorR1Pin);
  motorL1.attach(MotorL1Pin);
  motorR2.attach(MotorR2Pin);
  motorL2.attach(MotorL2Pin);
}

// controls code assumes robot will never be "boxed in"
// that is it will always have at least one free direction
void loop() {
    
    checkFront();
    checkRight(); 
    checkLeft();
    checkBack();
    
    if (isFrontObst) {
      if (isRightObst && isLeftObst) {
        // turn around
      }
      else if (!isRightObst && isLeftObst) {
        // turn right
      }
      else {
        // turn left
      }
    }
    else if (isBackObst) {
      if (isRightObst && isLeftObst) {
        // turn around
      }
      else if (!isRightObst && isLeftObst) {
        // turn right
      }
      else {
        // turn left
      }
    }
    else {
      // point all wheels straight, to go forward 
      motorR1.write(0);
      motorL1.write(0);
      motorR2.write(0);
      motorL2.write(0);
    }   
}

// IR SENSING - ASSUMES IR Returns 0 if noting is in proximity
// 1 otherwise

// Checks the front of the vehicle with its IR sensors and
// updates navigation status accordingly.
void checkFront() {
  int sensorValueL = analogRead(IR2_pin);
  int sensorValueR  = analogRead(IR1_pin);
  if (sensorValueL == 1 && sensorValueR == 1) {
    isFrontObst = true;
  }
  else {
    isFrontObst = false;
  }
}

// Checks the right of the vehicle with its IR sensors and
// updates navigation status accordingly.
void checkLeft() {
  // TODO: should add a state that reads whether robot is going forward
  // or backward and only check that IR sensor accordingly.
  
  int sensorValueL1 = analogRead(IR2_pin);
  int sensorValueL2  = analogRead(IR4_pin);
  if (sensorValueL1 == 1 && sensorValueL2 == 1) {
    isLeftObst = true;
  }
  else {
    isLeftObst = false;
  }
}

// Checks the left of the vehicle with its IR sensors and
// updates navigation status accordingly.
void checkRight() {
  // TODO: should add a state that reads whether robot is going forward
  // or backward and only check that IR sensor accordingly.
  
  int sensorValueR1 = analogRead(IR1_pin);
  int sensorValueR2  = analogRead(IR3_pin);
  if (sensorValueR1 == 1 && sensorValueR2 == 1) {
    isRightObst = true;
  }
  else {
    isRightObst = false;
  }

}

// Checks the back of the vehicle with its IR sensors and
// updates navigation status accordingly.
void checkBack() {
  int sensorValueL = analogRead(IR4_pin);
  int sensorValueR  = analogRead(IR3_pin);
  if (sensorValueL == 1 && sensorValueR == 1) {
    isBackObst = true;
  }
  else {
    isBackObst = false;
  }
}
