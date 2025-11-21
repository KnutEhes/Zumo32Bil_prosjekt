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
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;

int batteri = batteryPercent;
int leftSpeed = 100;
int rightSpeed = 100;
void startSkjerm(){
    display.clear();
    display.gotoXY(0, 0);
    display.print("Bilen er klar til kalibrering!");
    display.gotoXY(0, 1);
    display.print("Sett bilen på tapen og trykk på knapp A");
}
//Hva det skal stå etter kalibrering er fullført
void kalibrering(){
    display.clear();
    display.gotoXY(0, 0);
    display.print("Kalibrering fullført!");
    display.gotoXY(0, 1);
    display.print("Trykk knapp A for å starte linjefølger!");
}
// Viser hastighet og batteri når bilen kjører
void kjoring(){ 
    //Fart
    display.clear();
    display.gotoXY(0,1);
    display.print("Venstre");
    display.gotoXY(0, 2);
    display.print(leftSpeed);
    display.gotoXY(1, 1);
    display.print("Høyre");
    display.gotoXY(1, 2);
    display.print(rightSpeed);
    //Batteri
    display.gotoXY(0, 0);
    display.print("Batterinivå: ");
    display.gotoXY(1, 0);
    display.print(batteri);
    display.gotoXY(2, 0);
    display.print("%");
}
//Lyn-tegn når man lader
//const unsigned char PROGEM lyn[] = {
//0b00100,
//0b01100,
//0b01110,
//0b00110,
//0b00100,
//0b00000,
//0b00000,
//0b00000
//};
//Lagrer Lyn i slot 0
 

//Ladestasjon 
void ladestasjon(){
    display.clear();
    display.gotoXY(0, 0);
    display.print("Press B for hurtiglading");
    display.gotoXY(0, 1);
    display.print("Press C for vanlig lading");

    if(buttonB.isPressed()){ //hurtiglading
        display.clear();
        display.gotoXY(0, 0);
        display.print("Lader:");
        display.gotoXY(1, 0);
        display.print("Hurtig");
        display.gotoXY(0, 1);
        display.print(batteri);
        display.gotoXY(1, 1);
        display.print("%");
      //  display.gotoXY(10, 0);
       // display.drawBitmap(10, 0, lyn);
       // display.display();
    }
    else if(buttonC.isPressed()) { //Hvis vanlig lading
        display.clear();
        display.gotoXY(0, 0);
        display.print("Lader:");
        display.gotoXY(1, 0);
        display.print("Vanlig");
        display.gotoXY(0, 1);
        display.print(batteri);
        display.gotoXY(1, 1);
        display.print("%");
        display.gotoXY(10, 0);
        display.write((byte)0); //Lyn
    }
}





