#ifndef BUZZANDLED_H
#define BUZZANDLED_H

#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>

extern Zumo32U4Buzzer buzzer;
extern unsigned long lastBuzz;
extern int note;

void batteryBuzz();


#endif 
