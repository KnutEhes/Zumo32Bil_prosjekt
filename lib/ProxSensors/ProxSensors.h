#pragma once


#include <Zumo32U4.h>
#include <Arduino.h>

//Define her

extern bool objectDetected;
extern bool stopNow;
extern unsigned long detected;

extern int leftProxSensorValue;
extern int rightProxSensorValue;

const int limit = 5;

void proximitySense();

