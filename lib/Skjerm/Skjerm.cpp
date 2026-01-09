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

Zumo32U4OLED display;
extern Zumo32U4ButtonA buttonA;
extern Zumo32U4ButtonB buttonB;
extern Zumo32U4ButtonC buttonC;

int batteri = batteryPercent;
int leftSpeed = 100;
int rightSpeed = 100;
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
void kalibrering(){
    display.setLayout21x8();
    display.clear();
    display.gotoXY(0, 0);
    display.print("Kalibrering ferdig!");
    display.gotoXY(0, 1);
    display.print("Trykk knapp ");
    display.gotoXY(0, 2);
    display.print("for start!");
}
// Viser hastighet og batteri når bilen kjører
void kjoring(){ 
    display.setLayout21x8();
    //Fart
    display.clear();
    display.gotoXY(0, 0);
    display.print("Batteri: ");
    display.print(batteri);
    display.print("%");
    display.gotoXY(0,2);
    display.print("Left");
    display.gotoXY(0, 3);
    display.print(leftSpeed);
    display.gotoXY(15, 2);
    display.print("Right");
    display.gotoXY(15, 3);
    display.print(rightSpeed);
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

//Ladestasjon 
void ladestasjon(){
    display.setLayout21x8();
    display.clear();
    display.gotoXY(0, 0);
    display.print("Velg type lading");
    display.gotoXY(0, 1);
    display.print("Press B for hurtig");
    display.gotoXY(0, 2);
    display.print("Press C for vanlig");
}
    void hurtigLading(){
        display.clear();
        display.loadCustomCharacter(lyn, 0); //Lagrer lyn i slot 0
        display.gotoXY(0, 0);
        display.print("Lader: Hurtig");
        display.gotoXY(0, 1);
        display.print(batteri);
        display.print("%");
        
}
    void vanligLading(){
        display.clear();
        display.loadCustomCharacter(lyn, 0); //Lagrer lyn i slot 0
        display.gotoXY(0, 0);
        display.print("Lader: Vanlig");
        display.gotoXY(0, 1);
        display.print(batteri);
        display.print("%");      
}






