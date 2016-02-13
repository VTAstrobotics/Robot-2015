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
const int LEFT_DRIVE_PIN  = 0;
const int RIGHT_DRIVE_PIN = 1;
const int ACTUATOR1A_PIN  = 2;
const int ACTUATOR1B_PIN  = 3;
const int ACTUATOR2_PIN   = 4;

PWMTalon RIGHT_DRIVE_CONTROLLER;
PWMTalon LEFT_DRIVE_CONTROLLER;
PWMTalon ACTUATOR1A_CONTROLLER;
PWMTalon ACTUATOR1B_CONTROLLER;
PWMTalon ACTUATOR2_CONTROLLER;
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
    ACTUATOR1A_CONTROLLER.set_speed(0.0f);
    ACTUATOR1B_CONTROLLER.set_speed(0.0f);
    ACTUATOR2_CONTROLLER.set_speed(0.0f);
    dead = true;
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
        LEFT_DRIVE_CONTROLLER.set_speed(speed);
    } else if(data.id == DRIVE_RIGHT) {
        float speed = (data.val - 90) / 90.0f;
        RIGHT_DRIVE_CONTROLLER.set_speed(speed);
    } else if(data.id == ACTUATOR1_UP || data.id == ACTUATOR1_DOWN) {
        if(data.val && data.id == ACTUATOR1_UP) {
            ACTUATOR1A_CONTROLLER.set_speed(SPEED_ACTUATOR1_UP);
            ACTUATOR1B_CONTROLLER.set_speed(SPEED_ACTUATOR1_UP);
        } else if(data.val && data.id == ACTUATOR1_DOWN) {
            ACTUATOR1A_CONTROLLER.set_speed(SPEED_ACTUATOR1_DOWN);
            ACTUATOR1B_CONTROLLER.set_speed(SPEED_ACTUATOR1_DOWN);
        } else {
            ACTUATOR1A_CONTROLLER.set_speed(0.0f);
            ACTUATOR1B_CONTROLLER.set_speed(0.0f);
        }
    } else if(data.id == ACTUATOR2_UP || data.id == ACTUATOR2_DOWN) {
        float speed = (data.val - 90) / 90.0f;
        if(data.id == ACTUATOR2_DOWN) {
            speed *= -1.0f;
        }
        ACTUATOR2_CONTROLLER.set_speed(speed * SPEED_ACTUATOR2);
    }
}

void check_connected() {
    if(comm.isNetworkUp()) {
        digitalWrite(READY_LED, HIGH);
    } else {
        Serial.println("Ping timed out!");
        digitalWrite(READY_LED, LOW);
        killMotors();
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
    ACTUATOR1A_CONTROLLER.attach(ACTUATOR1A_PIN);
    ACTUATOR1B_CONTROLLER.attach(ACTUATOR1B_PIN);
    ACTUATOR2_CONTROLLER.attach(ACTUATOR2_PIN);
    killMotors();
}

void loop() {
    check_connected();
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
