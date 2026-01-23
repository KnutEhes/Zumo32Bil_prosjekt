#ifndef PROXSENSORS_H
#define PROXSENSORS_H

#include <Zumo32U4.h>
#include <Arduino.h>

//Define her

extern bool objectDetected;
extern bool stopNow;
extern unsigned long detected;

const int limit = 5;

void proximitySense();

#endif
