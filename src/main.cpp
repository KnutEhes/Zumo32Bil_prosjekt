#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>

#include "ProxSensors.h"
#include "init.h"
#include "LinjeSensor.h"
#include "buzzAndLed.h"
#include "Cream.h"
#include "motor.h"
#include "zumoBattery.h"


void setup() {
  
  initAll();
  
}


void loop() {

  proximitySense();
  zumoBatteri.batteryLoop(slowCharge, fastCharge, minIsbil);
  zumoBatteri.testIcles();
  iceCream();  
  setSpeeds();

}
