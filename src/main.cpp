#include <Arduino.h>
#include <Zumo32U4.h>
//#include "PID.h"
//#include "swBatteri.h"
#include "EVCharge.h"
#include "sensorRead.h"


// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  initProxSensor();
  //int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
  readSensors();
  proximitySense();

}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}