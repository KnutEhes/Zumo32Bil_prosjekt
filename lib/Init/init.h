#ifndef INIT_H
#define INIT_H

#include <Zumo32U4.h>
#include <Arduino.h>
#include <Wire.h>


extern Zumo32U4ProximitySensors proxSensor;

extern Zumo32U4ButtonA buttonA;
extern Zumo32U4ButtonB buttonB;
extern Zumo32U4ButtonC buttonC;
extern Zumo32U4Buzzer buzzer;

extern Zumo32U4OLED display;
extern Zumo32U4LineSensors lineSensors;

extern Zumo32U4Motors motors;



void initAll();

#endif