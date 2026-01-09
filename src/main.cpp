#include <Arduino.h>
#include <Zumo32U4.h>

#include "PID.h"
#include "swBatteri.h"
#include "EVCharge.h"
#include "sensorRead.h"

#include <Wire.h>



Zumo32U4OLED oled;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  initLineSensors();
  initProxSensor();
  initLineSensors()
  //int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:

  readSensors();
  proximitySense();
  battery();

  MotorSpeeds  = lineFollower();  
  motors.setSpeeds(s.left, s.right);

}
