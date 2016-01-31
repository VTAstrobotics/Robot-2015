#include "NetComm.h"
#include "crc-16.h"
#include <Servo.h>
#include <Arduino.h>
#include <Astrobotics_Shield_Header.h>
#include "common.h"

const int LOOP_HZ = 50;
const int LOOP_DELAY = (int) (1000 / LOOP_HZ);
const int READY_LED = 12;
const int ACTIVE_LED = 11;

// Motor controllers
const int RIGHT_DRIVE_PIN = 0;
const int LEFT_DRIVE_PIN = 1;

PWMTalon RIGHT_DRIVE_CONTROLLER;
PWMTalon LEFT_DRIVE_CONTROLLER;
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
    RIGHT_DRIVE_CONTROLLER.set_speed(0.0f);
    LEFT_DRIVE_CONTROLLER.set_speed(0.0f);
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

    if(data.id == DRIVE_LEFT) {
        float speed = (data.val - 90) / 90.0f;
        char out[64];
        sprintf(out, "Left value: %f", speed);
        Serial.println(out);
        LEFT_DRIVE_CONTROLLER.set_speed(speed);
    } else if(data.id == DRIVE_RIGHT) {
        float speed = (data.val - 90) / 90.0f;
        char out[64];
        sprintf(out, "Right value: %f", speed);
        Serial.println(out);
        RIGHT_DRIVE_CONTROLLER.set_speed(speed);
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
    PWMTalon::talon_init();
    LEFT_DRIVE_CONTROLLER.attach(LEFT_DRIVE_PIN, true);
    RIGHT_DRIVE_CONTROLLER.attach(RIGHT_DRIVE_PIN);
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
