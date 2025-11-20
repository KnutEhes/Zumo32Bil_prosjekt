#ifndef SWBATTERI_H
#define SWBATTERI_H

#include <Arduino.h>

// ----------------------
// Globale variabler
// ----------------------
extern int batteryCapacity;  
extern int currentCharge;    
extern int prevCharge;       

extern int balance;          

extern unsigned long currentTime;

extern bool fastCharging;
extern bool slowCharging;

// ----------------------
// Funksjonsprototyper
// ----------------------

// Hent batteriprosent (0–100)
float getBatteryPercent();

// Fast-charging pris som funksjon av tid
float getFastChargingPrice(unsigned long time);

// Slow-charging pris som funksjon av tid
float getSlowChargingPrice(unsigned long time);

// Oppdater helsen på batteriet
void batteryHealth();

// Hovedfunksjon for å håndtere lading/draining
void battery();

#endif
