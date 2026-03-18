#include "Communication.h"
//#include "Protocol.h"
#include "zumoBattery.h"

#define zumoAddress 0x55



void onRequest(){
    Wire.write((uint8_t*)&minIsbil, sizeof(minIsbil));
}

void commSetup(){
    Wire.begin(zumoAddress);
    Wire.onRequest(onRequest);
}