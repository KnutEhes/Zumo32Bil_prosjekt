#include "swBatteri.h"
#include "hovedModul.h"

Zumo32U4LCD lcd;

int batteryHealth = 100;
float batteryPersentVal = 100;
float prevBatteryPersentVal = 100;

float acceleration = 0;
int speed = 0;

unsigned long lastSpeedCheck = 0;

// Ladepriser som funskjon av tid
float fastChrargingPrice = 1.5 * sin(0.0008 * time + 5) + 8 + 0.00001 * time;
float slowChargingPrice = 1.5 * sin(0.001 * time + 13) + 5 + 0.00001 * time;

//Batteriet kan ikke lade opp høyere enn batteryHealth
void cycles()
{
    if((batteryPersentVal < 20 && prevBatteryPersentVal > 20) || (prevBatteryPersentVal > 80 && prevBatteryPersentVal < 80)){
        batteryHealth -= 1;
    }

}


void charging()
{
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