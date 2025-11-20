#include "swBatteri.h"
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
        batteryCapacity -= 5;
    }
    if (fastCharging)
    {
        batteryCapacity -= 10;
    }
}

void battery()
{
    currentTime = millis();
    unsigned long lastBatteryUpdate = 0;
    float chargePrice = 0;

    if (fastCharging)
    {
        chargePrice = getFastChargingPrice(currentTime);
        batteryHealth();

        while ((currentCharge < batteryCapacity) || (balance > 0)){

            if (currentTime - lastBatteryUpdate > 10){
                currentCharge += 1;
                balance -= chargePrice;
                lastBatteryUpdate = currentTime;
            }
        }

        fastCharging = false;
    }

    else if (slowCharging)
    {
        chargePrice = getSlowChargingPrice(currentTime);

        while ((currentCharge < batteryCapacity) || (balance > 0)){
            
            if (currentTime - lastBatteryUpdate > 50){
                currentCharge += 1;
                balance -= chargePrice;
                lastBatteryUpdate = currentTime;
            }
        }

        slowCharging = false;
    }

    else
    {
        if (currentTime - lastBatteryUpdate > 100)
        {
            currentCharge -= 2; // funksjon for batteriDraining her

            batteryHealth();

            prevCharge = currentCharge;
        }
    }

}


