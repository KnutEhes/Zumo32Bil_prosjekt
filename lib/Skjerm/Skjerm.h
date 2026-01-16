

#ifndef Skjerm_h
#define Skjerm_h

#include <Zumo32U4.h>
#include <Arduino.h>
#include <Wire.h>

//function declaration
void startSkjerm(); //Display startskjerm. Trykk knapp for kalibrering
void kalibrering(); // Kalibrering ferdig. trykk knapp for å starte kjøring
void kjoring(); //Display kjøring. Viser fart, batteri og penger
void ladestasjon(); // Valg av type lading
void hurtigLading(); //Display hurtigLading
void vanligLading(); //Display vanligLading
void displayChar(); //Ikke i bruk
void lyskryss(); //Display for lysskryss. rød, gul og grønn.
extern int batteri;
extern int leftSpeed;
extern int rightSpeed;
#endif // Skjerm_h





























//Kalle på funksjoner fra cpp
