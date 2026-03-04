#include <init.h>
#include "LinjeSensor.h"
#include "Cream.h"

Zumo32U4ProximitySensors proxSensor;

Zumo32U4Buzzer buzzer;

Zumo32U4LineSensors lineSensors;

Zumo32U4Motors motors;


void initAll(){

    Serial.begin(9600);
    proxSensor.initFrontSensor();
    initLineSensors();
    delay(20);
    calibrateSensors();
    creamSetup();


}
