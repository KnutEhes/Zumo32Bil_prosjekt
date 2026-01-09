

#ifndef Skjerm_h
#define Skjerm_h

#include <Zumo32U4.h>
#include <Arduino.h>
#include <Wire.h>

//function declaration
void startSkjerm();
void kalibrering();
void kjoring();
void ladestasjon();
void lyskryss();
void hurtigLading();
void vanligLading();
void displayChar();
extern int batteri;
extern int leftSpeed;
extern int rightSpeed;
#endif // Skjerm_h





























//Kalle på funksjoner fra cpp
