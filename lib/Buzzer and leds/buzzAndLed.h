#pragma once


#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>

extern unsigned long lastBuzz;
extern int note;

void batteryBuzz();
void iceCreamBuzz();
void kaChingBuzz();


