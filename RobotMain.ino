#include "NetComm.h"
#include "crc-16.h"
#include <Wire.h>

const int LOOP_HZ = 50;
const int LOOP_DELAY = (int) (1000 / LOOP_HZ);

NetComm comm;
ControlData control;
int speedL = 90;
int speedR = 90;

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
    if(data.id == 1) {
        speedL = data.val;
    } else if(data.id == 3) {
        speedR = data.val;
    } else {
        return;
    }
    byte speed[] = {speedL, speedL, speedR, speedR};
    Wire.beginTransmission(1);
    Wire.write(speed, 4);
    Wire.endTransmission();
}

void setup() {
    Serial.begin(9600);
    Wire.begin();
    // Initialize default values for control
    bzero(&control, sizeof(control));
}

void loop() {
    if(comm.getData(&control)) {
        motorControl(control);
    } else {
        delay(LOOP_DELAY);
    }
}
