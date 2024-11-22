#include "motor.h"

// #define LEFT_MOTOR_ENABLE_PIN 2
// #define RIGHT_MOTOR_ENABLE_PIN 3

// #define LEFT_MOTOR_IN1_PIN 2
// #define LEFT_MOTOR_IN2_PIN 2

// #define RIGHT_MOTOR_IN1_PIN 3
// #define RIGHT_MOTOR_IN2_PIN 3

void motorInit() {
    gpio_init(LEFT_MOTOR_ENABLE_PIN);
    gpio_set_dir(LEFT_MOTOR_ENABLE_PIN, GPIO_OUT);

    gpio_init(RIGHT_MOTOR_ENABLE_PIN);
    gpio_set_dir(RIGHT_MOTOR_ENABLE_PIN, GPIO_OUT);

    gpio_init(LEFT_MOTOR_IN1_PIN);
    gpio_set_dir(LEFT_MOTOR_IN1_PIN, GPIO_OUT);

    gpio_init(LEFT_MOTOR_IN2_PIN);
    gpio_set_dir(LEFT_MOTOR_IN2_PIN, GPIO_OUT);

    gpio_init(RIGHT_MOTOR_IN1_PIN);
    gpio_set_dir(RIGHT_MOTOR_IN1_PIN, GPIO_OUT);

    gpio_init(RIGHT_MOTOR_IN2_PIN);
    gpio_set_dir(RIGHT_MOTOR_IN2_PIN, GPIO_OUT);
}

void motorLeftOn() {
    gpio_put(LEFT_MOTOR_ENABLE_PIN, 1);
}

void motorRightOn() {
    gpio_put(RIGHT_MOTOR_ENABLE_PIN, 1);
}

void motorLeftOff() {
    gpio_put(LEFT_MOTOR_ENABLE_PIN, 0);
}

void motorRightOff() {
    gpio_put(RIGHT_MOTOR_ENABLE_PIN, 0);
}

void motorLeftForward();

void motorLeftBackward();

void motorRightForward();

void motorRightBackward();

void motorLeftCoast() {
    gpio_put(LEFT_MOTOR_IN1_PIN, 0);
    gpio_put(LEFT_MOTOR_IN2_PIN, 0);
}

void motorLeftBrake() {
    gpio_put(LEFT_MOTOR_IN1_PIN, 1);
    gpio_put(LEFT_MOTOR_IN2_PIN, 1);
}

void motorRightCoast() {
    gpio_put(RIGHT_MOTOR_IN1_PIN, 0);
    gpio_put(RIGHT_MOTOR_IN2_PIN, 0);
}

void motorRightBrake() {
    gpio_put(RIGHT_MOTOR_IN1_PIN, 1);
    gpio_put(RIGHT_MOTOR_IN2_PIN, 1);
}

void enableMotors() {
    motorLeftOn();
    motorRightOn();
}

void disableMotors() {
    motorLeftOff();
    motorRightOff();
}

void coastAll() {
    motorLeftCoast();
    motorRightCoast();
}

void brakeAll() {
    motorLeftBrake();
    motorRightBrake();
}

void forward(int time);

void backward(int time);

void turnLeft(int time);

void turnRight(int time);

void coast(int time);

void brake(int time);
