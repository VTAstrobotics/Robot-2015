/*
 * constants.h
 *
 *  Created on: Apr 10, 2015
 *      Author: Anirudh Bagde
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <time.h>

// Motor and misc values
static const int SPEED_ACTUATOR_UP   = 30;
static const int SPEED_ACTUATOR_DOWN = 130;
static const int SPEED_DUMP_BTN      = 135;
static const int SPEED_DIG_BTN       = 45;
static const int SPEED_DRUM_INC      = SPEED_ACTUATOR_DOWN;
static const double DRUM_INC_TIME    = 0.25;

// Xbox controller IDs
static const int LTHUMBX   = 0;
static const int LTHUMBY   = 1;
static const int RTHUMBX   = 2;
static const int RTHUMBY   = 3;
static const int RTRIGGER  = 4;
static const int LTRIGGER  = 5;
static const int A         = 6;
static const int B         = 7;
static const int X         = 8;
static const int Y         = 9;
static const int LB        = 10;
static const int RB        = 11;
static const int BACK      = 12;
static const int START     = 13;
static const int XBOX      = 14;
static const int LTHUMBBTN = 15;
static const int RTHUMBBTN = 16;
static const int DPAD      = 17;
// L2 and R2 are for controllers that have buttons instead of triggers
static const int L2        = 18;
static const int R2        = 19;

// Specific controls
// Axes
static const int DRIVE_LEFT  = LTHUMBY;
static const int DRIVE_RIGHT = RTHUMBY;
static const int DUMP        = LTRIGGER;
static const int DIG         = RTRIGGER;
// Buttons
static const int DRIVE_LEFT_STOP  = LTHUMBBTN;
static const int DRIVE_RIGHT_STOP = RTHUMBBTN;
static const int DUMP_BTN         = L2;
static const int DIG_BTN          = R2;
static const int ACTUATOR_UP      = Y;
static const int ACTUATOR_DOWN    = A;
static const int DEADMAN          = LB;
static const int DRUM_INCREMENT   = RB;

// Helper functions
inline double getSeconds(timespec time) {
    return time.tv_sec + (time.tv_nsec / 1.0e9);
}

inline double getCurrentSeconds() {
    timespec time;
    bzero(&time, sizeof(time));
    clock_gettime(CLOCK_MONOTONIC, &time);
    return getSeconds(time);
}

#endif /* COMMON_H_ */
