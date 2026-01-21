#include <Arduino.h>
#include <Zumo32U4.h>
#include "LinjeSensor.h"
#include "init.h"

const uint16_t maxSpeed = 200;
int16_t lastError = 0;
int16_t lastSpeedDifference = 0;
uint16_t crossingCount = 0;
bool robotWasOnCrossing = false; // VIKTIG: For å hindre dobbelttelling

const float Kp = 1.0 / 14.0;
const float Kd = 0.8 / 14.0;
const float filterAlpha = 0.55;

const uint8_t numSensors = 5;
uint16_t lineSensorValues[numSensors];

void initLineSensors() {
  lineSensors.initFiveSensors();
}

void calibrateSensors() {
  delay(1000);
  for(uint16_t i = 0; i < 120; i++) {
    if (i > 30 && i <= 90) motors.setSpeeds(-200, 200);
    else motors.setSpeeds(200, -200);
    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);
}

LinjesensorData getLineSensorData() {
  LinjesensorData data;
  // readLine returnerer posisjon (0-4000 for 5 sensorer)
  int16_t position = lineSensors.readLine(lineSensorValues);
  
  for(uint8_t i = 0; i < 5; i++) {
    data.sensorVerdi[i] = lineSensorValues[i];
  }

  const uint16_t threshold = 500;
  uint8_t sensorCount = 0;
  for(uint8_t i = 0; i < 5; i++) {
    if (lineSensorValues[i] > threshold) sensorCount++;
  }

  // Logikk for kryss-deteksjon
  bool isCrossingNow = (sensorCount >= 3);

  if (isCrossingNow && !robotWasOnCrossing) {
    // Vi har akkurat truffet et nytt kryss
    crossingCount++;
    robotWasOnCrossing = true; 
  } else if (!isCrossingNow) {
    // Vi har forlatt krysset
    robotWasOnCrossing = false;
  }

  data.kryss = isCrossingNow;
  data.kryss_count = crossingCount;
  data.kryss_posisjon = position;

  // Bestem antall grener (Forenklet logikk)
  if (sensorCount == 5) {
    data.grener = 4; // Fullt kryss
  } else if (isCrossingNow) {
    data.grener = 3; // T-kryss eller lignende
  } else {
    data.grener = 0;
  }

  return data;
}

MotorSpeeds lineFollower() {
  MotorSpeeds speeds;
  int16_t position = lineSensors.readLine(lineSensorValues);
  
  // Senter er 2000 for 5 sensorer (0, 1000, 2000, 3000, 4000)
  int16_t error = position - 2000;

  // PID - Proporsjonal + Derivat
  float pTerm = Kp * error;
  float dTerm = Kd * (error - lastError);
  
  int16_t rawDifference = (int16_t)(pTerm + dTerm);
  
  // Lavpassfilter for å unngå "jitter"
  int16_t speedDifference = (int16_t)(filterAlpha * rawDifference + (1.0 - filterAlpha) * lastSpeedDifference);
  
  lastError = error;
  lastSpeedDifference = speedDifference;

  int16_t leftSpeed = (int16_t)maxSpeed + speedDifference;
  int16_t rightSpeed = (int16_t)maxSpeed - speedDifference;

  // Begrens hastighet til Zumo-maks (400)
  speeds.left = constrain(leftSpeed, -400, 400);
  speeds.right = constrain(rightSpeed, -400, 400);
  
  return speeds;
}