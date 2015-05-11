#include "NetComm.h"
#include "constants.h"
#include "crc-16.h"
#include <Servo.h>
#include <Arduino.h>

const int LOOP_HZ = 50;
const int LOOP_DELAY = (int) (1000 / LOOP_HZ);
const int READY_LED = 12;
const int ACTIVE_LED = 11;

NetComm comm;
ControlData control;
int speedL = 90;
int speedR = 90;
int speedDrum = 90;
int speedActuator = 90;
bool dead = true;

Servo LEFT_DRIVE_CONTROLLER;
Servo RIGHT_DRIVE_CONTROLLER;
Servo DRUM_CONTROLLER;
Servo ACTUATOR_CONTROLLER;

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
    LEFT_DRIVE_CONTROLLER.write(speedL);
    RIGHT_DRIVE_CONTROLLER.write(speedR);
    DRUM_CONTROLLER.write(speedDrum);
    ACTUATOR_CONTROLLER.write(speedActuator);
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
            speedL = data.val;
            LEFT_DRIVE_CONTROLLER.write(speedL);
        } else if(data.id == DRIVE_RIGHT) {
            speedR = data.val;
            RIGHT_DRIVE_CONTROLLER.write(speedR);
        }
    }

    else if(data.id == DIG || data.id == DUMP || data.id == ACTUATOR_UP
            || data.id == ACTUATOR_DOWN) { //Drum and Arm
        if(data.id == DUMP) { // Drum control
            // Assuming this one is 90-180 and drive is 90-0
            speedDrum = data.val;
            DRUM_CONTROLLER.write(speedDrum);
        } else if(data.id == DIG) {
            speedDrum = -data.val + 180;
            DRUM_CONTROLLER.write(speedDrum);
        } else if(data.id == ACTUATOR_UP || data.id == ACTUATOR_DOWN) { // Actuator control
            if(data.val == 0) { // Stopped pressing button
                speedActuator = 90;
                ACTUATOR_CONTROLLER.write(speedActuator);
            } else if(data.id == ACTUATOR_UP) {
                speedActuator = SPEED_ACTUATOR_UP;
                ACTUATOR_CONTROLLER.write(speedActuator);
            } else if(data.id == ACTUATOR_DOWN) {
                speedActuator = SPEED_ACTUATOR_DOWN;
                ACTUATOR_CONTROLLER.write(speedActuator);
            }
        }
    }

    char out[64];
    sprintf(out, "Drive left: %d, Right: %\tDrum rotation: %d, actuator: %d",
            speedL, speedR, speedDrum, speedActuator);
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
    digitalWrite(ACTIVE_LED, !dead);
    // Initialize motor controllers
    LEFT_DRIVE_CONTROLLER.attach(LEFT_DRIVE_PIN);
    RIGHT_DRIVE_CONTROLLER.attach(RIGHT_DRIVE_PIN);
    DRUM_CONTROLLER.attach(DRUM_PIN);
    ACTUATOR_CONTROLLER.attach(ACTUATOR_PIN);
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
