#ifndef SWBATTERI_H
#define SWBATTERI_H

#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>


void cycles();


extern int batteryHealth;
extern float fastChrargingPrice;
extern float slowChrargingPrice;

const float kSpeed = 0.5;
const float kAcc = 1.2;

#endif