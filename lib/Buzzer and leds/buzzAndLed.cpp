#include "buzzAndLed.h"
#include "swBatteri.h"
#include "init.h"

unsigned long lastBuzz = 0;
unsigned long lastIceCreamBuzz = 0;
unsigned long lastSexyBuzz = 0;
unsigned long lastSonicBuzz = 0;
int note = 1;



void batteryBuzz(){
    
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

//Spiller iskrem-musikk
void iceCreamBuzz(){
    if (lastIceCreamBuzz+12000 < millis()){
        buzzer.play("!T140 L4 e16f+16g+16a>c+e.g+8a>c+e>d L8 >c+b>c+a>c+b>c+a L1 be8");
        Serial.println("Spiller musikk");
        lastIceCreamBuzz = millis();
    }
}
//Penge kling
void kaChingBuzz(){
    buzzer.play("!>a16>>e8");
}

void sexyBuzz(){
    if (lastSexyBuzz+12000 < millis()){
        buzzer.play(
        "!T80 L8"
        "E64 F64 G64"
        ">E >D16 A F."
        ">E >D16 A F."
        ">C A#16 F D."
        ">C A#16 F D."
        "A# A16 F D <A#2"
        "<A <A# C D E F G A"
        );
        lastSexyBuzz = millis(); 
    }

}

void sonicBuzz(){
    if (lastSonicBuzz+12000 < millis()){
        buzzer.play(
        "!T152 L16 O5 V15"
        "A F A F B G B G"
        ">C A >C A >D B >D B"
        "L4 O4"
        "B. A. B. A. B A"
        ">C. B. A"
        "L8 O2"
        "MS >C ML >C MS A ML A MS A# ML A# MS B B"
        "ML L4 O4"
        "A. B. >C A. B. >C"
        ">C. B1"
        );
        lastSonicBuzz = millis();
    }  
}