#include "zumoBattery.h"
#include "init.h"

Isbil minIsbil{1000.0, 50, 0.0};
Charger slowCharge(10.0, 1.0);
Charger fastCharge(20.0, 5.0);
Battery zumoBatteri;

// Flaggene som Battery::charging sjekker etter (extern i .cpp)
bool fastChargeActive = false;
bool slowChargeActive = false;

float Battery::getPercentage(){
    return (100 * (currentCharge / maxCapacity));
}

void Battery::batteryLoop(Charger& slowCharge, Charger& fastCharge, Isbil& zumo){
    if ((millis() - lastUpdate) > updateInterval){
        charging(slowCharge, fastCharge, zumo);
        draining(slowCharge, fastCharge, zumo);
        batteryHealth();
        lastUpdate = millis();
        minIsbil.batteryLevel = prevPercent = getPercentage();
        
    }
}


float Charger::getChargingPrice(unsigned long time){
    return 0.2 * sin(0.0001 * time) + pricePerLoop + 0.000001 * time; 
}

void Battery::charging(Charger& slowCharge, Charger& fastCharge, Isbil& zumo){

    if (fastChargeActive){
        currentCharge += fastCharge.getRate();
        zumo.balance -= fastCharge.getChargingPrice(millis());
        
    }
    else if (slowChargeActive){
        currentCharge += slowCharge.getRate();
        zumo.balance -= slowCharge.getChargingPrice(millis());

    }
    if ((fastChargeActive || slowChargeActive) && (currentCharge >= maxCapacity)){
        currentCharge = maxCapacity;
        slowChargeActive = fastChargeActive = false;
    }
    if ((fastChargeActive || slowChargeActive) && (zumo.balance <= 0)){
        zumo.balance = 0;
        slowChargeActive = fastChargeActive = false;
    }
}


void Battery::draining(Charger& slowCharge, Charger& fastCharge, Isbil& zumo){
    if (!fastChargeActive && !slowChargeActive){
        currentCharge -= 0.01 * zumo.speed;
        if (currentCharge <= 0) currentCharge = 0;
    }
}

void Battery::batteryHealth(){
    if (getPercentage() < 20 && prevPercent >= 20) maxCapacity -= 5;
    if (getPercentage() > 80 && prevPercent <= 80) maxCapacity -= 5;
    if (getPercentage() < 0.5 && prevPercent >= 0.5) maxCapacity -= 10;
    if (fastChargeActive) maxCapacity -= 1;
    if (maxCapacity < minCapacity) maxCapacity = minCapacity;
    if (zumoBatteri.getPercentage() < 10 && prevPercent >= 10){
        buzzer.playNote(NOTE_A(4), 200, 15);
    }
}
/*
void Battery::testIcles() {
    // Sett layout til 21 tegn per linje og 4 linjer (mindre skrift)
    // Dette fungerer på Zumo32U4OLED
    display.setLayout21x8(); 
    display.clear();
    
    // Rad 1: Batteri
    display.gotoXY(0, 0);
    display.print("Status: ");
    if (fastChargeActive) display.print("FastCharge");
    else if (slowChargeActive) display.print("SlowCharge");
    else display.print("Draining");

    // Rad 2: Prosent
    display.gotoXY(0, 1);
    display.print("Batteri: ");
    display.print(getPercentage(), 1);
    display.print("%");

    // Rad 3: Penger
    display.gotoXY(0, 2);
    display.print("Saldo:   ");
    display.print(minIsbil.balance, 2);
    display.print(" kr");

    // Rad 4: Is
    display.gotoXY(0, 3);
    display.print("Iskrem:  ");
    display.print(minIsbil.iceCreams);
}
*/