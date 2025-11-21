#include "swBatteri.h"
#include "buzzAndLed.h"

Zumo32U4OLED display;

// Batterikapasitet og nåværende ladning
float batteryCapacity = 1000.0;
float currentCharge = 1000.0;

// Prosentvis ladning
float batteryPercent = 100.0;
float prevBatteryPercent = 100.0;

// Hvor mye penger bilen har
float balance = 10000.0;

// Bilens fart (brukes for debugging)
//float speed = 100.0;

// Tidsvariabler for å styre oppdatering
unsigned long lastBatteryUpdate = 0;
unsigned long currentTime = 0;

// Lade-moduser
bool fastCharging = false;
bool slowCharging = false;

int batteryLvl = 0;

// ----------------------------------------------------------
// PROSENT
// ----------------------------------------------------------
float getBatteryPercent()
{
    if (batteryCapacity <= 0) return 0.0; // sikkerhetssjekk
    return (currentCharge / batteryCapacity) * 100.0;
}


// ----------------------------------------------------------
// PRISFUNKSJONER (pris for 1Wh)
// ----------------------------------------------------------
float getFastChargingPrice(unsigned long time)
{
    return 0.15 * sin(0.0008 * time + 5) + 0.8 + 0.00001 * time; 
}

float getSlowChargingPrice(unsigned long time)
{
    return 0.15 * sin(0.001 * time + 13) + 0.5 + 0.00001 * time;
}


// ----------------------------------------------------------
// BATTERIHEALTH (senker kapasitet ved slitasje)
// ----------------------------------------------------------
void batteryHealth(float newPercent, float oldPercent)
{
    // Går under 20%
    if (newPercent < 20 && oldPercent >= 20) batteryCapacity -= 5;

    // Går over 80%
    if (newPercent > 80 && oldPercent <= 80) batteryCapacity -= 5;

    // Treffer nesten 0%
    if (newPercent < 0.5 && oldPercent >= 0.5) batteryCapacity -= 10;

    // Hurtiglading gir ekstra slitasje
    if (fastCharging) batteryCapacity -= 0.0001 * fastChargeRate;

    // Ikke la kapasitet gå lavere enn minimum
    if (batteryCapacity < minBatteryCapacity) batteryCapacity = minBatteryCapacity;
}


// ----------------------------------------------------------
// DEBUG PRINT
// ----------------------------------------------------------
void debugPrint(){
    Serial.print("Capacity: "); Serial.print(batteryCapacity);
    Serial.print("Wh    Charge: ");     Serial.print(currentCharge);
    Serial.print("Wh    Balance: ");    Serial.print(balance);
    Serial.print("kr    Battery: ");    Serial.print(getBatteryPercent()); 
    Serial.print("%"); Serial.print("    SlowChargingPrice: "); Serial.print(getSlowChargingPrice(currentTime));
    Serial.print("kr    FastChargingPrice: "); Serial.print(getFastChargingPrice(currentTime)); Serial.println("kr");
    display.clear();
    display.print(batteryPercent);
    display.display();
    
}


// ----------------------------------------------------------
// HOVED BATTERISYSTEM
// ----------------------------------------------------------
void battery()
{
    currentTime = millis(); // hent tid
    float chargePrice = 0;  // pris per Wh


    // ------------------------------------------------------
    // FAST CHARGE
    // ------------------------------------------------------
    if (fastCharging)
    {
        chargePrice = getFastChargingPrice(currentTime); // hent pris for hurtiglading
        //speed = 0; // ikke kjør mens du lader (debugging)

        if (currentTime - lastBatteryUpdate >= updateInterval){
            
            currentCharge += fastChargeRate; // legg til energi
            if (currentCharge > batteryCapacity) currentCharge = batteryCapacity;

            balance -= chargePrice * fastChargeRate; // betal for lading
            if (balance < 0) balance = 0;

            // Stopp lading hvis full eller blakk
            if (currentCharge >= batteryCapacity || balance == 0){
                fastCharging = false;
                //speed = 100; // bilen kan kjøre igjen (debugging)
            }

            lastBatteryUpdate = currentTime;
        }
    }


    // ------------------------------------------------------
    // SLOW CHARGE
    // ------------------------------------------------------
    else if (slowCharging)
    {
        chargePrice = getSlowChargingPrice(currentTime); // pris for sakte lading
        //speed = 0; // ikke kjør mens du lader (debugging)

        if (currentTime - lastBatteryUpdate >= updateInterval){
            
            currentCharge += slowChargeRate; // legg til energi
            if (currentCharge > batteryCapacity) currentCharge = batteryCapacity;

            balance -= chargePrice * slowChargeRate; // betal for lading
            if (balance < 0) balance = 0;

            // Stopp lading hvis full eller blakk
            if (currentCharge >= batteryCapacity || balance == 0){
                slowCharging = false;
                //speed = 100; //(debugging)
            }

            lastBatteryUpdate = currentTime;
        }
    }


    // ------------------------------------------------------
    // NORMAL DRAINING
    // ------------------------------------------------------
    else
    {
        if (currentTime - lastBatteryUpdate >= 100)
        {
            // Tapp batteriet basert på fart
            currentCharge -= 300 / drainRate; //Midlertidig funksjon for drain

            if (currentCharge < 0) currentCharge = 0;

            /* Kode for Debugging
            if (currentCharge == 0) speed = 0;

            if (currentCharge < 100) fastCharging = true;
            */
            lastBatteryUpdate = currentTime;
        }
    }

    // Skriv ut status
    debugPrint();


    // Oppdater prosent og helse
    batteryPercent = getBatteryPercent();
    batteryBuzz();
    batteryHealth(batteryPercent, prevBatteryPercent);
    prevBatteryPercent = batteryPercent;
}

