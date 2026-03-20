#pragma once

#include <Arduino.h>
#include <Wire.h>

void commSetup(void);

struct zumoInstructions {   //struct til instruksjoner til bilen MÅ SAMSVARE MED master ESP
    bool trafficLightGreen;
    char nextTurn;
    char posName;

    zumoInstructions(bool l = true, char t = 0, char p = 0)
        : trafficLightGreen(l), nextTurn(t), posName(p) {}
};
