#include "NetComm.h"
#include "constants.h"
#include "crc-16.h"
#include <Wire.h>
#include <Arduino.h>

const int LOOP_HZ = 50;
const int LOOP_DELAY = (int) (1000 / LOOP_HZ);
const int READY_LED = 13;

NetComm comm;
ControlData control;
int speedL = 90;
int speedR = 90;
int speedDrum = 90;

void printData(ControlData& data) {
    char out[64];
    if(data.id == 17) {
        sprintf(out, "DPAD, Y: %d, X: %d", data.dpadY, data.dpadX);
    } else {
        sprintf(out, "Data update, id: %d, value: %d", data.id, data.val);
    }
    Serial.println(out);
}

void motorControl(ControlData& data) {
    // Update state
    if(data.id == DRIVE_LEFT) { // Drivetrain
        speedL = data.val;
    } else if(data.id == DRIVE_RIGHT) {
        speedR = data.val;
    } else if(data.id == DIG) { // Drum control
        // Assuming this one is 90-180 and dump is 90-0
        speedDrum = data.val;
    } else if(data.id == DUMP) {
        speedDrum = -data.val + 180;
    } else {
        return;
    }

    // Send state to motor slaves
    byte speed[] = { speedL, speedL, speedR, speedR };
    Wire.beginTransmission(ADDR_DRIVE_SLAVE);
    Wire.write(speed, 4);
    Wire.endTransmission();

    char out[64];
    sprintf(out, "Drive left: %d, right: %d, Drum speed: %d", speedL, speedR, speedDrum);
    Serial.println(out);
}

void setup() {
    Serial.begin(9600);
    Wire.begin();
    // Initialize default values for control
    bzero(&control, sizeof(control));
    // Activate LED to indicate readiness
    pinMode(READY_LED, OUTPUT);
    digitalWrite(READY_LED, HIGH);
}

void loop() {
    if(comm.getData(&control)) {
        printData(control);
        motorControl(control);
    } else {
        delay(LOOP_DELAY);
    }
}
