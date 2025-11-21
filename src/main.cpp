#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>

#include "EVCharge.h"
#include "sensorRead.h"
#include "PID.h"
#include "swBatteri.h"
#include "init.h"


void setup() {
  
  initAll();

}

void loop() {
  
  readSensors();
  proximitySense();
  battery();

}
