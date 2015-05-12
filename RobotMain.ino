#include "NetComm.h"
#include "constants.h"
#include "crc-16.h"
#include <Servo.h>
#include <Arduino.h>

const int LOOP_HZ = 50;
const int LOOP_DELAY = (int) (1000 / LOOP_HZ);
const int READY_LED = 12;
const int ACTIVE_LED = 11;

// Motor controllers
const int RIGHT_DRIVE_PIN = 10;
const int DRUM_PIN = 9;
const int ACTUATOR_PIN = 6;
const int LEFT_DRIVE_PIN = 5;
const int OFFSET_LEFT = -4;
const int OFFSET_RIGHT = -4;
const int OFFSET_DRUM = -6;
const int OFFSET_ACTUATOR = -5;

Servo RIGHT_DRIVE_CONTROLLER;
Servo LEFT_DRIVE_CONTROLLER;
Servo ACTUATOR_CONTROLLER;
Servo DRUM_CONTROLLER;
NetComm comm;
ControlData control;
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
    RIGHT_DRIVE_CONTROLLER.write(90 + OFFSET_RIGHT);
    LEFT_DRIVE_CONTROLLER.write(90 + OFFSET_LEFT);
    ACTUATOR_CONTROLLER.write(90 + OFFSET_ACTUATOR);
    DRUM_CONTROLLER.write(90 + OFFSET_DRUM);
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
    if(data.id == DRIVE_LEFT || data.id == DRIVE_RIGHT) { // Drivetrain
        if(data.id == DRIVE_LEFT) {
            LEFT_DRIVE_CONTROLLER.write(data.val + OFFSET_LEFT);
        } else if(data.id == DRIVE_RIGHT) {
            RIGHT_DRIVE_CONTROLLER.write(data.val + OFFSET_RIGHT);
        }
    }

    else if(data.id == DIG || data.id == DUMP || data.id == ACTUATOR_UP
            || data.id == ACTUATOR_DOWN || data.id == DIG_BTN
            || data.id == DUMP_BTN) { // Drum and Arm
        if(data.id == DUMP) { // Drum control
            // Assuming this one is 90-180 and dig is 90-0
            DRUM_CONTROLLER.write(data.val + OFFSET_DRUM);
        } else if(data.id == DIG) {
            DRUM_CONTROLLER.write(-data.val + 180 + OFFSET_DRUM);
        } else if(data.id == DUMP_BTN || data.id == DIG_BTN) { // Drum control when using non-Xbox controllers
            if(data.val == 0) {
                DRUM_CONTROLLER.write(90 + OFFSET_DRUM);
            } else {
                if(data.id == DUMP_BTN) {
                    DRUM_CONTROLLER.write(SPEED_DUMP_BTN + OFFSET_DRUM);
                } else if(data.id == DIG_BTN) {
                    DRUM_CONTROLLER.write(SPEED_DIG_BTN + OFFSET_DRUM);
                }
            }
        } else if(data.id == ACTUATOR_UP || data.id == ACTUATOR_DOWN) { // Actuator control
            if(data.val == 0) { // Stopped pressing button
                ACTUATOR_CONTROLLER.write(90 + OFFSET_ACTUATOR);
            } else {
                if(data.id == ACTUATOR_UP) {
                    ACTUATOR_CONTROLLER.write(SPEED_ACTUATOR_UP + OFFSET_ACTUATOR);
                } else if(data.id == ACTUATOR_DOWN) {
                    ACTUATOR_CONTROLLER.write(SPEED_ACTUATOR_DOWN + OFFSET_ACTUATOR);
                }
            }
        }
    }
}

void check_ready() {
    static bool led_set = false;
    if(led_set) {
        return;
    }
    if(comm.isNetworkUp()) {
        led_set = true;
        digitalWrite(READY_LED, HIGH);
    }
}

void setup() {
    Serial.begin(9600);
    // Initialize default values for control
    bzero(&control, sizeof(control));
    // Activate LED to indicate readiness
    pinMode(READY_LED, OUTPUT);
    pinMode(ACTIVE_LED, OUTPUT);
    // Initialize motor controllers
    RIGHT_DRIVE_CONTROLLER.attach(RIGHT_DRIVE_PIN);
    LEFT_DRIVE_CONTROLLER.attach(LEFT_DRIVE_PIN);
    ACTUATOR_CONTROLLER.attach(ACTUATOR_PIN);
    DRUM_CONTROLLER.attach(DRUM_PIN);
    killMotors();
}

void loop() {
    check_ready();
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
