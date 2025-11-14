#include "swBatteri.h"
#include "hovedModul.h"

int batteryHealth = 100;

//Ladepriser som funskjon av tid
float fastChrargingPrice = 1.5 * sin(0.0008 * time + 5) + 8 + 0.00001 * time;
float slowChargingPrice = 1.5 * sin(0.001 * time + 13) + 5 + 0.00001 * time;

void cycles()
{
}

