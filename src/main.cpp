#include <Arduino.h>
#include <Zumo32U4.h>

#include "LinjeSensor.h"
#include "swBatteri.h"
#include "EVCharge.h"
#include "sensorRead.h"
#include "Cream.h"

#include <Wire.h>
#include <Skjerm.h>

Zumo32U4Motors motors;
Zumo32U4OLED oled;

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
  
  // Viser startskjerm og venter på kalibrering
  startSkjerm();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  readSensors();
  proximitySense();
  battery();
  iceCream();

  MotorSpeeds s = lineFollower(); 
  motors.setSpeeds(s.left,s.right);
  
  SkjermKjoring();


}