// PID.cpp  — Optimalisert for maksimal flyt (smoothness)
#include <Arduino.h>
#include <Zumo32U4.h>
#include "LinjeSensor.h"
#include "init.h"

const uint16_t maxSpeed = 200;
int16_t lastError = 0;
int16_t lastSpeedDifference = 0;
uint16_t crossingCount = 0;    

// VERDIER FOR SMOOTH KJØRING:
const float Kp = 1.1 / 14.0;    // Litt ned fra 1.2 for å unngå nervøs styring
const float Kd = 3.5 / 14.0;    // Økt betydelig (fra 2.0) for å fjerne all risting
const float filterAlpha = 0.6; // Senket fra 0.7 for mye mykere overganger (viktig!)

const uint8_t numSensors = 5;
uint16_t lineSensorValues[numSensors];

void initLineSensors() {
  lineSensors.initFiveSensors();
}

void calibrateSensors() {
  delay(1000);
  for(uint16_t i = 0; i < 120; i++)
  {
    if (i > 30 && i <= 90) motors.setSpeeds(-200, 200);
    else motors.setSpeeds(200, -200);
    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);
}

LinjesensorData getLineSensorData() {
  LinjesensorData data;
  int16_t position = lineSensors.readLine(lineSensorValues);
  for(uint8_t i = 0; i < 5; i++) {
    data.sensorVerdi[i] = lineSensorValues[i];
  }
  const uint16_t threshold = 500;
  uint8_t sensorCount = 0;
  bool left = lineSensorValues[0] > threshold;
  bool center_left = lineSensorValues[1] > threshold;
  bool center = lineSensorValues[2] > threshold;
  bool center_right = lineSensorValues[3] > threshold;
  bool right = lineSensorValues[4] > threshold;
  
  if (left) sensorCount++;
  if (center_left) sensorCount++;
  if (center) sensorCount++;
  if (center_right) sensorCount++;
  if (right) sensorCount++;
  
  if (sensorCount >= 3) {
    data.kryss = true;
    data.kryss_posisjon = position;
    if (center && left && right) data.grener = 4;
    else if ((center && left && center_right) || (center && center_left && right)) data.grener = 3;
    else data.grener = 2;
    crossingCount++;
    data.kryss_count = crossingCount;
  } else {
    data.kryss = false;
    data.kryss_posisjon = 0;
    data.grener = 0;
    data.kryss_count = crossingCount;
  }
  return data;
}

MotorSpeeds lineFollower() {
  MotorSpeeds speeds;
  int16_t position = lineSensors.readLine(lineSensorValues);
  int16_t error = position - 2000;

  // PID
  int16_t speedDifference = (int16_t)(Kp * error + Kd * (error - lastError));
  
  // Lavpassfilter - Nå mer aggressivt for å glatte ut bevegelsen
  speedDifference = (int16_t)(filterAlpha * speedDifference + (1 - filterAlpha) * lastSpeedDifference);
  
  if (speedDifference > 200) speedDifference = 200;
  if (speedDifference < -200) speedDifference = -200;

  lastError = error;
  lastSpeedDifference = speedDifference;

  int16_t leftSpeed = (int16_t)maxSpeed + speedDifference;
  int16_t rightSpeed = (int16_t)maxSpeed - speedDifference;

  if (leftSpeed > 400) leftSpeed = 400;
  if (leftSpeed < -400) leftSpeed = -400;
  if (rightSpeed > 400) rightSpeed = 400;
  if (rightSpeed < -400) rightSpeed = -400;

  speeds.left = leftSpeed;
  speeds.right = rightSpeed;
  return speeds;
}