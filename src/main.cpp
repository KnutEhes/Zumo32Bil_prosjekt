#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>

#include "PID.h"
#include "swBatteri.h"

Zumo32U4OLED oled;

void setup() {
  Serial.begin(9600);
}

void loop() {
  battery();
  

}
