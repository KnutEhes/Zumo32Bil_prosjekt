#include "buzzAndLed.h"
#include "swBatteri.h"

Zumo32U4Buzzer buzzer;

unsigned long lastBuzz = 0;
int note = 1;

void batteryBuzz(){

    if ((batteryPercent) < 5 && (!fastCharging) && (!slowCharging)){
        if ((currentTime - lastBuzz) > 500){
            if (note == 1){
                buzzer.playNote(NOTE_E(4), 100, 5);
                note += 1;
            }
            else if (note == 2){
                buzzer.playNote(NOTE_G(6), 100, 5);
                note -= 1;
            }
            lastBuzz = currentTime;
        }
    }

    if (batteryPercent < 10 && prevBatteryPercent >= 10){
        buzzer.playNote(NOTE_A(9), 50, 5);
    }
}