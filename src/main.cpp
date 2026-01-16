#include <Arduino.h>
#include <Zumo32U4.h>

#include "PID.h"
#include "swBatteri.h"
#include "EVCharge.h"
#include "sensorRead.h"

#include <Wire.h>
#include <Skjerm.h>
#include "PID.h"
#include "swBatteri.h"


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //initProxSensor();
  initLineSensors();
  //int result = myFunction(2, 3);
  buttonA.waitForButton();
  buttonB.waitForButton();
  buttonC.waitForButton();
}

void loop() {
  // put your main code here, to run repeatedly:

  readSensors();
  proximitySense();
  battery();
  if(buttonA.isPressed()){
  startSkjerm();
  delay(10000);
  kalibrering();
  delay(10000);
  kjoring();
  delay(10000);
  ladestasjon();
  delay(10000);
  hurtigLading();
  delay(10000);
  vanligLading();
}
  

}