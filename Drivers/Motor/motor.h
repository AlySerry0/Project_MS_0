#ifndef MOTOR_H
#define MOTOR_H

#include "pico/stdlib.h"

#define LEFT_MOTOR_ENABLE_PIN 17
#define RIGHT_MOTOR_ENABLE_PIN 18

#define LEFT_MOTOR_IN1_PIN 19
#define LEFT_MOTOR_IN2_PIN 20

#define RIGHT_MOTOR_IN1_PIN 21
#define RIGHT_MOTOR_IN2_PIN 22

// Function prototypes
void motorInit();
void motorLeftOn();
void motorRightOn();
void motorLeftOff();
void motorRightOff();
void motorLeftForward();
void motorLeftBackward();
void motorRightForward();
void motorRightBackward();
void motorLeftCoast();
void motorLeftBrake();
void motorRightCoast();
void motorRightBrake();
void enableMotors();
void disableMotors();
void coastAll();
void brakeAll();

#endif