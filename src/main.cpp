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
  
  Serial.println("hello");
  initProxSensor();
  initLineSensors();
  delay(20);
  calibrateSensors();
  //initCompass();
  //Serial.print(startHeading());
  SkjermKalibrering();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  readSensors();
  proximitySense();
  battery();

  MotorSpeeds s = lineFollower(); 
  motors.setSpeeds(s.left,s.right);
  //Serial.print(startHeading());

  SkjermKjoring();


}