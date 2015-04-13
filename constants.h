/*
 * constants.h
 *
 *  Created on: Apr 10, 2015
 *      Author: Anirudh Bagde
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// Xbox controller IDs
static const int LTHUMBX = 0;
static const int LTHUMBY = 1;
static const int RTHUMBX = 2;
static const int RTHUMBY = 3;
static const int RTRIGGER = 4;
static const int LTRIGGER = 5;
static const int A = 6;
static const int B = 7;
static const int X = 8;
static const int Y = 9;
static const int LB = 10;
static const int RB = 11;
static const int BACK = 12;
static const int START = 13;
static const int XBOX = 14;
static const int LEFTTHUMB = 15;
static const int RIGHTTHUMB = 16;
static const int DPAD = 17;

// Specific controls
// Axes
static const int DRIVE_LEFT = LTHUMBY;
static const int DRIVE_RIGHT = RTHUMBY;
static const int DIG = RTRIGGER;
static const int DUMP = LTRIGGER;
// Buttons
static const int DRIVE_LEFT_STOP = LEFTTHUMB;
static const int DRIVE_RIGHT_STOP = RIGHTTHUMB;
static const int KILL_ALL = BACK;

// I2C addresses
static const int ADDR_DRIVE_SLAVE = 1;
static const int ADDR_DRUM_SLAVE = 2;

#endif /* CONSTANTS_H_ */
