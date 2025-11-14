#ifndef HOVEDMODUL_H
#define HOVEDMODUL_H

#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>

void readSensorValues();

void initZumo(); //alt som skal i setup


const float CPR = 909.7;
const float wheelCircumference = 0.10053;

extern uint8_t leftValue;
extern uint8_t rightValue;

extern unsigned long time;

struct Motion {
    float distance;
    float speed;
    float acceleration;
};

Motion calculateMotion();

#endif