//definer funksjoner

// Vise batterinivå
// Vise at den trenger å lade
// Vise meny og ladestatus (type lading kjøpt og prosent)
// Saldo og transaksjoner i forbindelse med kjøp av lading
// Vise hastighet
// Klar til / ferdig med kalibrering
//-----------------------------------------------------------------------------------------------

// Hva som skal stå på skjermen når bilen skrus på og skal til å kalibreres

#include "Skjerm.h"
#include "swBatteri.h"
#include "sensorRead.h"
#include "init.h"
#include "LinjeSensor.h"



void startSkjerm(){
    display.setLayout21x8();
    display.clear();
    display.gotoXY(0, 0);
    display.print("Bilen er klar");
    display.gotoXY(0, 1);
    display.print("til kalibrering");
    display.gotoXY(0, 3);
    display.print("Trykk knapp A");
}
//Hva det skal stå etter kalibrering er fullført
void SkjermKalibrering(){
    display.setLayout21x8();
    display.clear();
    display.gotoXY(0, 0);
    display.print("Kalibrerer");

}
// Viser hastighet og batteryPercent når bilen kjører
void SkjermKjoring(){ 
    display.setLayout21x8();
    MotorSpeeds screen = lineFollower(); 
    //Fart
    display.clear();
    display.gotoXY(0, 0);
    display.print("Batteri: ");
    display.print(batteryPercent);
    display.print("%");
    display.gotoXY(0, 6);
    //Serial.print(balance);
    display.print(balance);
    display.print("kr");
    display.gotoXY(0,2);
    display.print("Left");
    display.gotoXY(0, 3);
    display.print(screen.left);
    display.gotoXY(15, 2);
    display.print("Right");
    display.gotoXY(15, 3);
    display.print(screen.right);
// if(stoppet ved ladestasjon){
//SkjermLadestasjon();
//}
}

//Ladestasjon 
void SkjermLadestasjon(){
    display.setLayout21x8();
    display.clear();
    display.gotoXY(0, 6);
    display.print(balance);
    display.print("kr");
    display.gotoXY(0, 0);
    display.print("Velg type lading");
    display.gotoXY(0, 1);
    display.print("Press A for hurtig");
    display.gotoXY(0, 2);
    display.print("Press B for vanlig");

    if (buttonA.isPressed()){
        hurtigLading();
    }
    if (buttonB.isPressed()){
        vanligLading();
    }
}
    void hurtigLading(){
        display.clear();
        //display.drawBitmap(20, 10, lightningBitmap, 8, 16, 1);
        display.gotoXY(0, 0);
        display.print("Lader: Hurtig");
        display.gotoXY(0, 1);
        display.print(batteryPercent);
        display.print("%");
        display.gotoXY(0, 6);
        display.print(balance);
        display.print("kr");
        
}
    void vanligLading(){
        display.clear();
       // display.drawBitmap(20, 10, lightningBitmap, 8, 16, 1);
        display.gotoXY(0, 0);
        display.print("Lader: Vanlig");
        display.gotoXY(0, 1);
        display.print(batteryPercent);
        display.print("%"); 
        display.gotoXY(0, 6);
        display.print(balance);
        display.print("kr");     
}
void SkjermLyskryss(){
    display.clear();
    if("lys er rødt"){
        display.gotoXY(0, 0);
        display.print("Rødt");
        display.gotoXY(0, 1);
        display.print("Stopp!");
    }
    else if("lys er gult"){
        display.gotoXY(0, 0);
        display.print("Gult");
        display.gotoXY(0, 1);
        display.print("Brems!");
    }
    else if ("lys er rødt og gult"){
        display.gotoXY(0, 0);
        display.print("Rødt og gult");
        display.gotoXY(0, 1);
        display.print("Gjør deg klar til å kjøre");
    }
    else{
        display.gotoXY(0, 0);
        display.print("Grønt");
        display.gotoXY(0, 1);
        display.print("Kjør!");
    }
    
}
//Lyn-tegn når man lader
static const char lyn[] PROGMEM = {
    0b00100,
    0b01100,
    0b11100,
    0b00111,
    0b00110,
    0b00100,
    0b00000,
    0b00000
};
const uint8_t lightningBitmap[] = { 
    0b00000110, 
    0b00001100, 
    0b00011000, 
    0b00110000, 
    0b01100000, 
    0b11000000, 
    0b01100000, 
    0b00110000, 
    0b00011000, 
    0b00001100, 
    0b00000110, 
    0b00001100, 
    0b00011000, 
    0b00110000, 
    0b01100000, 
    0b11000000 
};






