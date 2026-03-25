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

// Midlertidige posisjoner for testing av driveTo-flyt i hovedkoden.
pos bilPos = {0.0f, 0.0f, 'F'};
pos targetPos = {1.0f, 0.0f, 'F'};

struct ZumoInstructions{
  bool trafficLightState;
  char nextTurn;
  
  ZumoInstructions(uint8_t t = 0, char n = 'F')
    : trafficLightState(t), nextTurn(n) {}
};

void setup() {
  
  initAll();
  
}


void loop() {

  proximitySense();
  zumoBatteri.batteryLoop(slowCharge, fastCharge, minIsbil);
  iceCream();  
  driveTo(bilPos, targetPos);
  zumoBatteri.testIcles();

}
