#include <EVCharge.h>
#include <sensorRead.h>

unsigned long chargeCoolDownValue = 0;
unsigned long chargeCoolDownTime = 10000;

bool chargerPresent = false;
bool fastCharging;
bool slowCharging;



void proximitySense(){
    if (rightProxSensorValue > 4 && chargeCoolDownValue < millis()-chargeCoolDownTime){     //lader i nærheten
        Serial.println("Lader oppdaget");
        chargerPresent = true;
        if (buttonAValue){   //hurtiglading
            fastCharging = true;
            Serial.println("lader fort...");
            delay(1000);
            fastCharging = false;
            chargeCoolDownValue = millis();
        }
        if (buttonBValue){   //sakte lading
            slowCharging = true;
            Serial.println("Lader sakteee....");
            delay(2000);
            slowCharging = false;
            chargeCoolDownValue = millis();
        }
        if (buttonCValue){   //avbrytlading
            Serial.println("ikke lad");
            chargerPresent = false;
            chargeCoolDownValue = millis();
        }
    }



}