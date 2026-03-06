/*
#pragma once

#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>


// ----------------------------------------------------------
// GLOBALE VARIABLER
// ----------------------------------------------------------
const float minBatteryCapacity = 200.0;
extern float batteryCapacity;
extern float currentCharge;

const float fastChargeRate = 10.0;
const float slowChargeRate = 3.0;
const float drainRate = 30.0;

const float speedDrainRate = 0.2;
const float accDrainRate = 0.2;

extern float batteryPercent;
extern float prevBatteryPercent;

extern float balance;

extern float testspeed;

const unsigned long updateInterval = 200;
extern unsigned long lastBatteryUpdate;
extern unsigned long currentTime;

extern bool fastCharging;
extern bool slowCharging;

extern int batteryLvl;

// ----------------------------------------------------------
// FUNKSJONER
// ----------------------------------------------------------

// Batteriprosent
float getBatteryPercent();

// Prisfunksjoner
float getFastChargingPrice(unsigned long time);
float getSlowChargingPrice(unsigned long time);

// Helseberegning
void batteryHealth(float newPercent, float oldPercent);

// Debug-utskrift
void debugPrint();

// Hovedsystem – kall denne i loop()
void battery();
*/