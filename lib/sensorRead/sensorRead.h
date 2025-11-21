//Leser av knapper og proximity-sensor

#ifndef __SENSORREAD_H__
#define __SENSORREAD_H__

#include <Zumo32U4.h>
#include <Arduino.h>

void readSensors(void);
void initProxSensor(void);

extern int rightProxSensorValue;
extern int leftProxSensorValue;
extern bool buttonAValue;
extern bool buttonBValue;
extern bool buttonCValue;


#endif