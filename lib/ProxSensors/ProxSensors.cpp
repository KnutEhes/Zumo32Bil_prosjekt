#include "ProxSensors.h"
#include "init.h"





bool proximitySense(){
    bool objectDetected = false;
    unsigned long detected = 0;
    int leftProxSensorValue = 0;
    int rightProxSensorValue = 0;
    bool shouldStop = false;
    leftProxSensorValue = proxSensor.countsFrontWithLeftLeds();
    rightProxSensorValue = proxSensor.countsFrontWithRightLeds();

    if ((rightProxSensorValue > limit || leftProxSensorValue > limit) && !objectDetected){    
        objectDetected = true;
        detected = millis();
    }
    if (((millis() - detected) > 50) && (rightProxSensorValue > limit || leftProxSensorValue > limit) && objectDetected){
        shouldStop = true;
    }
    if ((rightProxSensorValue < limit || leftProxSensorValue < limit) && objectDetected){    
        objectDetected = false;
        detected = millis();
    }
    if (((millis() - detected) > 50) && (rightProxSensorValue < limit || leftProxSensorValue < limit) && !objectDetected){
        shouldStop = false;
    }
    return shouldStop;
/*
    Serial.print(objectDetected);
    Serial.print(stopNow);
    Serial.print(rightProxSensorValue);
    Serial.println(leftProxSensorValue);

*/

}
