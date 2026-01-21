#include "sensorRead.h"
#include "init.h"
//Leser av de tre knappene samt. proximity sensor


bool buttonAValue;
bool buttonBValue;
bool buttonCValue;

int leftProxSensorValue;
int rightProxSensorValue;



void readSensors(){
    proxSensor.read();
    buttonAValue = buttonA.getSingleDebouncedPress();
    buttonBValue = buttonB.getSingleDebouncedPress();
    buttonCValue = buttonC.getSingleDebouncedPress();

    leftProxSensorValue = proxSensor.countsFrontWithLeftLeds();
    rightProxSensorValue = proxSensor.countsFrontWithRightLeds();


    
}