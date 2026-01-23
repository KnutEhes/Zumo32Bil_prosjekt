#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>

#include "ProxSensors.h"
#include "sensorRead.h"
#include "swBatteri.h"
#include "init.h"
#include "LinjeSensor.h"
#include "buzzAndLed.h"
#include "Skjerm.h"
#include "Cream.h"
#include "motor.h"

unsigned long screenUpdate = 0;
unsigned long timeNow = 0;


void setup() {
  
  initAll();
  
}



void loop() {
  timeNow = millis();

  readSensors();
  proximitySense();
  battery();
  iceCream();  
  setSpeeds();

  if ((timeNow - screenUpdate) > 400){
    SkjermKjoring();
    screenUpdate = timeNow;
  }

  


}
