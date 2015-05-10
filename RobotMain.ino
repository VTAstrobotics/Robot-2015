#include "NetComm.h"
#include "constants.h"
#include "crc-16.h"
#include <Servo.h>
#include <Arduino.h>

const int LOOP_HZ = 50;
const int LOOP_DELAY = (int) (1000 / LOOP_HZ);
const int READY_LED = 12;
const int ACTIVE_LED = 11;
const int RIGHT_DRIVE_PIN = 10;
const int DRUM_PIN = 9;
const int ACTUATOR_PIN = 6;
const int LEFT_DRIVE_PIN = 3;

Servo RIGHT_DRIVE_CONTROLLER;
Servo LEFT_DRIVE_CONTROLLER;
Servo ACTUATOR_CONTROLLER;
Servo DRUM_CONTROLLER;
NetComm comm;
ControlData control;
int speedL = 90;
int speedR = 90;
int speedDrum = 90;
int speedActuator = 90;
bool sendDrive = false;
bool sendDrum = false;
bool dead = true;

void printData(ControlData& data) {
    char out[64];
    if(data.id == 17) {
        sprintf(out, "DPAD, Y: %d, X: %d", data.dpadY, data.dpadX);
    } else {
        sprintf(out, "Data update, id: %d, value: %d", data.id, data.val);
    }
    Serial.println(out);
}

void killMotors() {
    speedL = speedR = speedDrum = speedActuator = 90;
    sendDrive = sendDrum = false;
    RIGHT_DRIVE_CONTROLLER.write(speedR);
    LEFT_DRIVE_CONTROLLER.write(speedL);
    ACTUATOR_CONTROLLER.write(speedActuator);
    DRUM_CONTROLLER.write(speedDrum);
  }

void motorControl(ControlData& data) {
    // Update state
    if(data.id == DEADMAN) {
        // Pressed = active, released = dead
        dead = !data.val;
        digitalWrite(ACTIVE_LED, data.val);
        if(dead) {
            killMotors();
        }
    }
    if(dead) {
        return;
    }
    if(data.id == DRIVE_LEFT || data.id == DRIVE_RIGHT) { //drivetrain
        if(data.id == DRIVE_LEFT) {
            LEFT_DRIVE_CONTROLLER.write(data.val);
        } else if(data.id == DRIVE_RIGHT) {
            RIGHT_DRIVE_CONTROLLER.write(data.val);
        }
    }

    else if(data.id == DIG || data.id == DUMP || data.id == ACTUATOR_UP
            || data.id == ACTUATOR_DOWN) { //Drum and Arm
        if(data.id == DUMP) { // Drum control
            // Assuming this one is 90-180 and drive is 90-0
            DRUM_CONTROLLER.write(data.val);
        } else if(data.id == DIG) {
            DRUM_CONTROLLER.write(-data.val + 180);
        } else if(data.id == ACTUATOR_UP || data.id == ACTUATOR_DOWN) { // Actuator control
            if(data.val == 0) { // Stopped pressing button
                ACTUATOR_CONTROLLER.write(90);
            } else if(data.id == ACTUATOR_UP) {
                ACTUATOR_CONTROLLER.write(SPEED_ACTUATOR1_UP);
            } else if(data.id == ACTUATOR_DOWN) {
                ACTUATOR_CONTROLLER.write(SPEED_ACTUATOR1_DOWN);
            }
        }
    }

    char out[64];
    sprintf(out, "Drive left: %d, right: %d, Drum speed: %d", speedL, speedR,
            speedDrum);
    Serial.println(out);
}

void setup() {
    Serial.begin(9600);
    // Initialize default values for control
    bzero(&control, sizeof(control));
    // Activate LED to indicate readiness
    pinMode(READY_LED, OUTPUT);
    pinMode(ACTIVE_LED, OUTPUT);
    digitalWrite(READY_LED, HIGH);
    RIGHT_DRIVE_CONTROLLER.attach(RIGHT_DRIVE_PIN);
    LEFT_DRIVE_CONTROLLER.attach(LEFT_DRIVE_PIN);    
    ACTUATOR_CONTROLLER.attach(ACTUATOR_PIN);
    DRUM_CONTROLLER.attach(DRUM_PIN);
  }

void loop() {
    if(comm.getData(&control)) {
        if(!dead) {
            printData(control);
        }
        motorControl(control);
    } else {
        if(dead) {
            Serial.println("dead");
        }
        delay(LOOP_DELAY);
    }
}
