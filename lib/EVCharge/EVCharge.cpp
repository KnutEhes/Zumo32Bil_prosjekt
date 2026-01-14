#include <EVCharge.h>
#include <sensorRead.h>
#include <swBatteri.h>

unsigned long chargeCoolDownValue = 0;
unsigned long chargeCoolDownTime = 10000;

bool chargerPresent = false;

//ladedetektering

void proximitySense(){
    if (rightProxSensorValue > 4 && chargeCoolDownValue < millis()-chargeCoolDownTime){     //lader i nærheten
        Serial.println("Lader oppdaget");
        chargerPresent = true;
        if (buttonAValue){   //hurtiglading
            fastCharging = true;
            chargerPresent = false;
            chargeCoolDownValue = millis();
        }
        if (buttonBValue){   //sakte lading
            slowCharging = true;
            chargerPresent = false;
            chargeCoolDownValue = millis();
        }
        if (buttonCValue){   //avbrytlading
            chargerPresent = false;
            chargeCoolDownValue = millis();
        }
    }



}