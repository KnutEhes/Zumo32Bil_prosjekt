#include "swBatteri.h"
#include "hovedModul.h"
/*
Batterikapasiteten er 1000Wh/1kWh
Prisfunksjon sier pris for 1Wh
Hvert 10ms endres batteri% og saldo for fastcharge
Hvert 50ms endres batteri% og saldo for slowcharge
BatteriDraining regnes ut av speedfunkjson fra Knut
Når x krav møtes skal batterikapasiteten synke fordi batterihealth synker
Saldo starter på x og synker når man lader

På skjermen skal saldo og batteriprosent vises
*/

int batteryCapacity = 1000; // max kapasitet på batteri i Wh
int currentCharge = 1000;   // hvor mange Wh som er igjen på batteriet
int prevCharge = 1000;      // hvor mange Wh som var i batteriet forrige runde i loop

int balance = 10000; // slado i kr

unsigned long lastBatteryUpdate = 0;
unsigned long currentTime = 0;

bool fastCharging = false;
bool slowCharging = false;

// funksjon for å hente batteriProsent for å vise på skjermen
float getBatteryPercent()
{
    return currentCharge / batteryCapacity * 100;
}

// funksjon for å hente fastCharging prisen når man lader
float getFastChargingPrice(unsigned long time)
{
    return 1.5 * sin(0.0008 * time + 5) + 0.8 + 0.00001 * time;
}

// funksjon for å hente slowCharging prisen når man lader
float getSlowChargingPrice(unsigned long time)
{
    return 1.5 * sin(0.001 * time + 13) + 0.5 + 0.00001 * time;
}

// funksjon for å endre på kapasiteten hvis batteriet lades høyt eller utlades lavt
void batteryHealth()
{
    if ((currentCharge < 20 && prevCharge > 20) || (currentCharge < 80 && prevCharge > 80))
    {
        batteryCapacity -= 3;
    }
    if (fastCharging){
        
    }
}

void batteryDrain()
{
    currentTime = millis();
    if (!fastCharging && !slowCharging)
    {
        if (currentTime - lastBatteryUpdate > 100)
        {
            currentCharge -= 0; // funksjon for batteriDraining her

            batteryHealth();

            prevCharge = currentCharge;
        }
    }
}

void batteryCharge()
{
    while ((currentCharge < batteryCapacity) || (balance > 0)){
        if (fastCharging)
        {
            
        }
        else if (slowCharging)
        {

        }
    }
}

/*


int batteryHealth = 100;
float batteryPersentVal = 100;
float prevBatteryPersentVal = 100;

float balance = 1000;
unsigned long lastBalanceCheck = 0;

float acceleration = 0;
int speed = 0;

unsigned long lastSpeedCheck = 0;

// Ladepriser som funskjon av tid
float fastChrargingPrice = 1.5 * sin(0.0008 * time + 5) + 8 + 0.00001 * time; //pris for 10%
float slowChargingPrice = 1.5 * sin(0.001 * time + 13) + 5 + 0.00001 * time; //

//Batteriet kan ikke lade opp høyere enn batteryHealth
void cycles()
{
    if((batteryPersentVal < 20 && prevBatteryPersentVal > 20) || (prevBatteryPersentVal > 80 && prevBatteryPersentVal < 80)){
        batteryHealth -= 1;
    }

}


void charging()
{
    if(charging){
        if (millis() - lastCharge > 20){

        }
        saldo = saldo -
    }
}

float distanceL = 0;

void batteryPercent()
{
    if (time - lastSpeedCheck > 1000)
    {
        Motion M = calculateMotion();
        batteryPersentVal -= kSpeed* abs(M.speed)+ kAcc* abs(M.acceleration);

        cycles();

        lcd.clear();
        lcd.gotoXY(0,0);
        lcd.println(batteryPersentVal);
        lcd.println(M.acceleration);
        lcd.println(M.speed);
        lcd.println(M.distance);

        prevBatteryPersentVal = batteryPersentVal;
        lastSpeedCheck = time;
    }
}

*/