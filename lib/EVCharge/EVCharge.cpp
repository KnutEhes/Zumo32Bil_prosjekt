#include <EVCharge.h>
#include <sensorRead.h>

unsigned long chargeCoolDownValue;
unsigned long chargeCoolDownTime = 10000;

bool chargerPresent = false;

void proximitySense(){

    if (rightProxSensorValue > 4 && chargeCoolDownValue > millis()-chargeCoolDownTime){     //lader i nærheten
        chargerPresent = true;
        if (buttonAValue){   //hurtiglading

        }
        if (buttonBValue){   //sakte lading

        }
        if (buttonCValue){   //avbrytlading
            chargerPresent = false;
            chargeCoolDownValue = millis();
        }
    }



}