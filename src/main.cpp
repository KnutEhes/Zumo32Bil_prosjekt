#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>

#include "ProxSensors.h"
#include "swBatteri.h"
#include "init.h"
#include "LinjeSensor.h"
#include "buzzAndLed.h"
#include "Cream.h"
#include "motor.h"


void setup() {
  
  initAll();
  
}


void loop() {

  proximitySense();
  battery();
  iceCream();  
  setSpeeds();

}
