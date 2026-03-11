#include "ProxSensors.h"
#include "motor.h"
#include "init.h"


const int wiggleRoom = 10;
bool objectDetected = false;
unsigned long lastChangeTime = 0;
bool shouldStop = false; 

bool proximitySense() {
    proxSensor.read();
    int leftProx = proxSensor.countsFrontWithLeftLeds();
    int rightProx = proxSensor.countsFrontWithRightLeds();
    bool sensorTriggered = (rightProx > limit || leftProx > limit);

    if (abs(rightSpeed - leftSpeed) < wiggleRoom) {
        
        if (sensorTriggered != objectDetected) {
            objectDetected = sensorTriggered;
            lastChangeTime = millis(); 
        }

        if ((millis() - lastChangeTime) > 50) {
            shouldStop = objectDetected; 
        }
    } else {
        
    }

    return shouldStop;
}
