#include "buzzAndLed.h"
#include "swBatteri.h"
#include "init.h"

unsigned long lastBuzz = 0;
int note = 1;



void batteryBuzz(){
    
    if (batteryPercent < 50 && prevBatteryPercent >= 50){
        buzzer.play("T152 L4 e16f+16g+16a>c+eg+8a+8>c+e>d L8 >c+b>c+a>c+b>c+a L1 be");
    }

    if ((batteryPercent) < 5 && (!fastCharging) && (!slowCharging)){
        if ((currentTime - lastBuzz) > 100){
            if (note == 1){
                buzzer.playNote(NOTE_E(6), 100, 15);
                note += 1;
            }
            else if (note == 2){
                buzzer.playNote(NOTE_G(6), 100, 15);
                note -= 1;
            }
            lastBuzz = currentTime;
        }
    }

    if (batteryPercent < 10 && prevBatteryPercent >= 10){
        buzzer.playNote(NOTE_A(4), 200, 15);
    }
}

