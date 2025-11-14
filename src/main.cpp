#include <Arduino.h>
#include <Zumo32U4.h>

#include "swBatteri.h"
#include "hovedModul.h"



void setup() {
  initZumo();
}

void loop() {
  readSensorValues();
  batteryPercent();
}
