#include "ProxSensors.h"
#include "sensorRead.h"
#include "swBatteri.h"
#include "init.h"


bool objectDetected = false;
bool stopNow = false;

unsigned long detected = 0;


void proximitySense(){
    if ((rightProxSensorValue > 4 || leftProxSensorValue > 4) & !objectDetected){    
        objectDetected = true;
        detected = millis();
    }
    if (((detected - millis()) > 50) & (rightProxSensorValue > 4 || leftProxSensorValue > 4) & objectDetected){
        stopNow = true;
    }
    if ((rightProxSensorValue < 4 || leftProxSensorValue < 4) & objectDetected){    
        objectDetected = false;
        detected = millis();
    }
    if (((detected - millis()) > 50) & (rightProxSensorValue < 4 || leftProxSensorValue < 4) & !objectDetected){
        stopNow = false;
    }

    Serial.print(objectDetected);
    Serial.print(stopNow);
    Serial.print(rightProxSensorValue);
    Serial.println(leftProxSensorValue);



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