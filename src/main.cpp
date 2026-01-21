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



void setup() {
  
  initAll();
  
}



void loop() {

  readSensors();
  proximitySense();
  battery();
  iceCream();  
  SkjermKjoring();

  
  if (stopNow){
        motors.setSpeeds(0, 0);
  }

  else{
        MotorSpeeds s = lineFollower(); 
        motors.setSpeeds(s.left,s.right);
  }

}
