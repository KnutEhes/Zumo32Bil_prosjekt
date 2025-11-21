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
Zumo32U4OLED display;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;

int batteriNivå = getBatteryPercent
void startSkjerm(){
    display.clear();
    displaygotoXY(0, 0);
    display.print("Bilen er klar til kalibrering!");
    displaygotoXY(0, 1);
    display.print("Sett bilen på tapen og trykk på knapp A")
}
//Hva det skal stå etter kalibrering er fullført
void kalibrering(){
    display.clear();
    displaygotoXY(0, 0);
    display.print("Kalibrering fullført!");
    displaygotoXY(0, 1);
    display.print("Trykk knapp A for å starte linjefølger!");
}
// Viser hastighet og batteri når bilen kjører
void kjøring(){ 
    //Fart
    display.clear();
    displaygotoXY(0,1);
    display.print("Venstre")
    displaygotoXY(0, 2);
    display.print(leftSpeed);
    displaygotoXY(1, 1);
    display.print("Høyre");
    displaygotoXY(1, 2);
    display.print(rightSpeed);
    //Batteri
    displaygotoXY(0, 0);
    display.print("Batterinivå: ")
    displaygotoXY(1, 0);
    display.print(batteriNivå);
    displaygotoXY(2, 0);
    display.print("%")
}
//Lyn-tegn når man lader
byte Lyn[8] = {
0b00100,
0b01100,
0b01110,
0b00110,
0b00100,
0b00000,
0b00000,
0b00000
};
//Lagrer Lyn i slot 0
lcd.createChar(0, Lyn); 

//Ladestasjon 
void ladestasjon(){
    display.clear();
    displaygotoXY(0, 0);
    display.print("Press B for hurtiglading");
    displaygotoXY(0, 1);
    display.print("Press C for vanlig lading");

    if(buttonB.isPressed()){ //hurtiglading
        display.clear();
        displaygotoXY(0, 0);
        display.print("Lader:");
        displaygotoXY(1, 0);
        display.print("Hurtig");
        displaygotoXY(0, 1);
        display.print(batteriNivå);
        displaygotoXY(1, 1);
        dispaly.print("%");
        displaygotoXY(10, 0);
        display.write((byte)0); //Lyn
    }
    else if(buttonC.isPressed()) { //Hvis vanlig lading
        display.clear();
        displaygotoXY(0, 0);
        display.print("Lader:");
        displaygotoXY(1, 0);
        display.print("Vanlig");
        displaygotoXY(0, 1);
        display.print(batteriNivå);
        displaygotoXY(1, 1);
        dispaly.print("%");
        displaygotoXY(10, 0);
        display.write((byte)0); //Lyn
    }
}
// Lyskryss
void lyskryss(){
    if(-----------------){ //Hvis det er rødt
        displaygoto(8, 0);
        display.print("Rødt");
    }
    else if(------------) { //Hvis det er gult
        displaygotoXY(8, 0);
        display.print("Gult");
    }
    else { //Hvis det er grønnt
        displaygotoXY(8, 0);
        display.print("Grønnt");
    }
}




