#include "motor.h"

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

void motorLeftForward(){
    gpio_put(LEFT_MOTOR_IN1_PIN, 1);
    gpio_put(LEFT_MOTOR_IN2_PIN, 0);
}

void motorLeftBackward(){
    gpio_put(LEFT_MOTOR_IN1_PIN, 0);
    gpio_put(LEFT_MOTOR_IN2_PIN, 1);
}

void motorRightForward(){
    gpio_put(RIGHT_MOTOR_IN1_PIN, 1);
    gpio_put(RIGHT_MOTOR_IN2_PIN, 0);
}

void motorRightBackward(){
    gpio_put(RIGHT_MOTOR_IN1_PIN, 0);
    gpio_put(RIGHT_MOTOR_IN2_PIN, 1);
}

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