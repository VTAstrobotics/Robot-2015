#include "NetComm.h"
#include "constants.h"
#include "crc-16.h"
#include <Wire.h>
#include <Arduino.h>

const int I2C_DELAY = 5;
const int LOOP_HZ = 50;
const int LOOP_DELAY = (int) (1000 / LOOP_HZ);
const int READY_LED = 13;
const int ACTIVE_LED = 12;

NetComm comm;
ControlData control;
int speedL = 90;
int speedR = 90;
int speedDrum = 90;
int speedActuator1 = 90;
int speedActuator2 = 90;
bool dead = true;
bool sendDrive = false;
bool sendDrum = false;

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
    speedL = speedR = speedDrum = speedActuator1 = speedActuator2 = 90;
    sendDrive = sendDrum = false;

    byte speed[] = { 90, 90, 90, 90 };

    Wire.beginTransmission(ADDR_DRIVE_SLAVE);
    Wire.write(speed, 4);
    Wire.endTransmission();

    Wire.beginTransmission(ADDR_DRUM_SLAVE);
    Wire.write(speed, 4);
    Wire.endTransmission();
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
        } else if(data.id == DRIVE_RIGHT) {
            speedR = data.val;
        }
        sendDrive = true;
    }

    else if(data.id == DIG || data.id == DUMP || data.id == ACTUATOR_UP
            || data.id == ACTUATOR_DOWN) { //Drum and Arm
        if(data.id == DIG) { // Drum control
            // Assuming this one is 90-180 and dump is 90-0
            speedDrum = data.val;
        } else if(data.id == DUMP) {
            speedDrum = -data.val + 180;
        } else if(data.id == ACTUATOR_UP || data.id == ACTUATOR_DOWN) { // Actuator control
            if(data.val == 0) { // Stopped pressing button
                speedActuator1 = 90;
                speedActuator2 = 90;
            } else if(data.id == ACTUATOR_UP) {
                speedActuator1 = SPEED_ACTUATOR1_UP;
                speedActuator2 = SPEED_ACTUATOR2_UP;
            } else if(data.id == ACTUATOR_DOWN) {
                speedActuator1 = SPEED_ACTUATOR1_DOWN;
                speedActuator2 = SPEED_ACTUATOR2_DOWN;
            }
        }
        sendDrum = true;
    }

    char out[64];
    sprintf(out, "Drive left: %d, right: %d, Drum speed: %d", speedL, speedR,
            speedDrum);
    Serial.println(out);
}

void sendMotorSlaves() {
    if(sendDrive) {
        byte speed[] = { speedL, speedL, speedR, speedR };
        Wire.beginTransmission(ADDR_DRIVE_SLAVE);
        Wire.write(speed, 4);
        Wire.endTransmission();
        sendDrive = false;
        Serial.println("sending drive");
    }

    delay(I2C_DELAY);

    if(sendDrum) {
        byte drum[] = { 0, speedDrum, speedActuator1, speedActuator2 };
        Wire.beginTransmission(ADDR_DRUM_SLAVE);
        Wire.write(drum, 4);
        Wire.endTransmission();
        sendDrum = false;
        Serial.println("sending drum");
    }
}

void setup() {
    Serial.begin(9600);
    Wire.begin();
    // Initialize default values for control
    bzero(&control, sizeof(control));
    // Activate LED to indicate readiness
    pinMode(READY_LED, OUTPUT);
    pinMode(ACTIVE_LED, OUTPUT);
    digitalWrite(READY_LED, HIGH);
}

void loop() {
    sendMotorSlaves();
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
