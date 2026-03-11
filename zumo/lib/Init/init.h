#pragma once


#include <Zumo32U4.h>
#include <Arduino.h>
#include <Wire.h>


extern Zumo32U4ProximitySensors proxSensor;

extern Zumo32U4Buzzer buzzer;

extern Zumo32U4LineSensors lineSensors;

extern Zumo32U4Motors motors;

extern Zumo32U4OLED display;

void initAll();

