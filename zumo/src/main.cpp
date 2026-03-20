#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>

#include "driveTo.h"
#include "ProxSensors.h"
#include "init.h"
#include "LinjeSensor.h"
#include "buzzAndLed.h"
#include "Cream.h"
#include "motor.h"
#include "zumoBattery.h"

struct ZumoInstructions {
  uint8_t trafficLightState;
  char nextTurn;

  ZumoInstructions(uint8_t t = 0, char n = 'F')
    : trafficLightState(t), nextTurn(n) {}
};

ZumoInstructions instructions;

void setup() {
  
  initAll();
  
}


void loop() {

  proximitySense();
  zumoBatteri.batteryLoop(slowCharge, fastCharge, minIsbil);
  iceCream();  
  driveTo(instructions.nextTurn);

}
