#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>

#include "driveTo.h"
#include "ProxSensors.h"
#include "swBatteri.h"
#include "init.h"
#include "LinjeSensor.h"
#include "buzzAndLed.h"
#include "Cream.h"
#include "motor.h"

// Midlertidige posisjoner for testing av driveTo-flyt i hovedkoden.
pos bilPos = {0.0f, 0.0f, 'F'};
pos targetPos = {1.0f, 0.0f, 'F'};

void setup() {
  
  initAll();
  
}


void loop() {
  
  battery();
  iceCream();  
  driveTo(bilPos, targetPos);

}
