#ifndef HOVEDMODUL_H
#define HOVEDMODUL_H

#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>

void readSensorValues();

void initZumo(); //alt som skal i setup

extern uint8_t leftValue;
extern uint8_t rightValue;

extern unsigned long time;



#endif