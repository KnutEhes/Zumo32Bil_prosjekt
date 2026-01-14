#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>

#include "EVCharge.h"
#include "sensorRead.h"
#include "swBatteri.h"
#include "init.h"
#include "PID.h"
#include "buzzAndLed.h"




void setup() {
  
  initAll();

}



void loop() {

  readSensors();
  proximitySense();
  battery();

  MotorSpeeds s = lineFollower(); 
  motors.setSpeeds(s.left,s.right);

}
