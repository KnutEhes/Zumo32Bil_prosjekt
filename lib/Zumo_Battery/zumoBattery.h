#pragma once

#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>

extern bool fastChargeActive;
extern bool slowChargeActive;

// 1. Move Charger and Isbil UP so Battery can see them
class Charger {
private:
    float rate;
    float pricePerLoop;
public:
    Charger(float chargeRate, float price) 
        : rate(chargeRate), pricePerLoop(price) {}
    
    float getRate() const { return rate; } // Helper to access private member
    float getChargingPrice(unsigned long time);
};

struct Isbil {
    float balance;
    int iceCreams;
    float speed;

    // Fix: Add parameters to the constructor so it actually sets values
    Isbil(float b = 0, int i = 0, float s = 0) 
        : balance(b), iceCreams(i), speed(s) {}
};

class Battery {
private:
    const float minCapacity = 200.0;
    float maxCapacity = 1000.0; // Added default value
    float currentCharge = 1000.0; // Added default value
    float prevPercent = 100.0;

public:
    float getPercentage();
    void charging(Charger& slowCharge, Charger& fastCharge, Isbil& zumo);
    void draining(Charger& slowCharge, Charger& fastCharge, Isbil& zumo);
    void batteryLoop(Charger& slowCharge, Charger& fastCharge, Isbil& zumo);
    void batteryHealth();
    //void testIcles();
    unsigned long lastUpdate = 0;
    const unsigned long updateInterval = 200;
};



extern Isbil minIsbil; 
extern Battery zumoBatteri;
extern Charger slowCharge;
extern Charger fastCharge;
