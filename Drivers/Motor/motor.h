#ifndef MOTOR_H
#define MOTOR_H

#include "pico/stdlib.h"

#define LEFT_MOTOR_ENABLE_PIN 2
#define RIGHT_MOTOR_ENABLE_PIN 3

#define LEFT_MOTOR_IN1_PIN 2
#define LEFT_MOTOR_IN2_PIN 2

#define RIGHT_MOTOR_IN1_PIN 3
#define RIGHT_MOTOR_IN2_PIN 3

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
void forward(int time);
void backward(int time);
void turnLeft(int time);
void turnRight(int time);
void coast(int time);
void brake(int time);

#endif