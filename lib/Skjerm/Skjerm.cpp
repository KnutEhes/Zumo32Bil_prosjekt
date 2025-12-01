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
    display.gotoXY(0,1);
    display.print("Left");
    display.gotoXY(0, 2);
    display.print(leftSpeed);
    display.gotoXY(15, 1);
    display.print("Right");
    display.gotoXY(15, 2);
    display.print(rightSpeed);
}
//Lyn-tegn når man lader
const uint8_t lyn[5] = {
  0b00000000,  // column 0
  0b00100000,  // column 1
  0b01100000,  // column 2
  0b01110000,  // column 3
  0b10011000   // column 4
};


//Ladestasjon 
void ladestasjon(){
    display.setLayout21x8();
    display.clear();
    display.gotoXY(0, 0);
    display.print("Press B for hurtig");
    display.gotoXY(0, 1);
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
        display.gotoXY(15, 0);
        
        for (int i = 0; i < 5; i++)
            display.write(lyn[i]);   // draw raw bitmap columns

        display.display();
    }
    void vanligLading(){
        display.clear();
        display.loadCustomCharacter(lyn, 0); //Lagrer lyn i slot 0
        display.gotoXY(0, 0);
        display.print("Lader: Vanlig");
        display.gotoXY(0, 1);
        display.print(batteri);
        display.print("%");
        display.gotoXY(15, 0);

        for (int i = 0; i < 5; i++)
            display.write(lyn[i]);   // draw raw bitmap columns

        display.display();
    }





