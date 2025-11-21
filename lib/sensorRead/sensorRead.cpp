#include "sensorRead.h"

//Leser av de tre knappene samt. proximity sensor

Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;

Zumo32U4ProximitySensors proxSensor;

bool buttonAValue;
bool buttonBValue;
bool buttonCValue;

int leftProxSensorValue;
int rightProxSensorValue;

void initProxSensor(){
  proxSensor.initFrontSensor();
}


void readSensors(){
    proxSensor.read();
    buttonAValue = buttonA.getSingleDebouncedPress();
    buttonBValue = buttonB.getSingleDebouncedPress();
    buttonCValue = buttonC.getSingleDebouncedPress();

    leftProxSensorValue = proxSensor.countsFrontWithLeftLeds();
    rightProxSensorValue = proxSensor.countsFrontWithRightLeds();

}