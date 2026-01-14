#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>

#include "ProxSensors.h"
#include "sensorRead.h"
#include "swBatteri.h"
#include "init.h"
#include "PID.h"
#include "buzzAndLed.h"
#include "speedcontroll.h"




void setup() {
  
  initAll();

}



void loop() {

  readSensors();
  proximitySense();
  battery();
  setSpeed();

}
