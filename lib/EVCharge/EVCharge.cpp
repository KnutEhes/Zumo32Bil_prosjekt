#include "EVCharge.h"
#include "sensorRead.h"
#include "swBatteri.h"
#include "init.h"

unsigned long chargeCoolDownValue = 0;
unsigned long chargeCoolDownTime = 10000;

bool chargerPresent = false;


void proximitySense(){
    if (rightProxSensorValue > 4 && chargeCoolDownValue < millis()-chargeCoolDownTime){     //lader i nærheten
        chargerPresent = true;
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
    



}