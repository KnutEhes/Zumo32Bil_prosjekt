#include "ProxSensors.h"
#include "sensorRead.h"
#include "swBatteri.h"
#include "init.h"


bool objectDetected = false;
bool stopNow = false;
unsigned long detected = 0;


void proximitySense(){
    if ((rightProxSensorValue > limit || leftProxSensorValue > limit) & !objectDetected){    
        objectDetected = true;
        detected = millis();
    }
    if (((detected - millis()) > 50) & (rightProxSensorValue > limit || leftProxSensorValue > limit) & objectDetected){
        stopNow = true;
    }
    if ((rightProxSensorValue < limit || leftProxSensorValue < limit) & objectDetected){    
        objectDetected = false;
        detected = millis();
    }
    if (((detected - millis()) > 50) & (rightProxSensorValue < limit || leftProxSensorValue < limit) & !objectDetected){
        stopNow = false;
    }
/*
    Serial.print(objectDetected);
    Serial.print(stopNow);
    Serial.print(rightProxSensorValue);
    Serial.println(leftProxSensorValue);

*/

}


/*

if (buttonAValue){   //hurtiglading
            fastCharging = true;
        }
        if (buttonBValue){   //sakte lading
            slowCharging = true;
        }
        if (buttonCValue){   //avbrytlading
            chargerPresent = false;
        }
    }
    

*/