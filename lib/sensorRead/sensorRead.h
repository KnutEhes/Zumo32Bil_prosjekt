//Leser av knapper og proximity-sensor

#ifndef __SENSORREAD_H__
#define __SENSORREAD_H__

#include <Zumo32U4.h>
#include <Arduino.h>


extern Zumo32U4ButtonA buttonA;
extern Zumo32U4ButtonB buttonB;
extern Zumo32U4ButtonC buttonC;

void readSensors(void);

extern int rightProxSensorValue;
extern int leftProxSensorValue;
extern bool buttonAValue;
extern bool buttonBValue;
extern bool buttonCValue;


#endif