// PID.cpp  — black line on white background
#include <Arduino.h>
#include <Zumo32U4.h>
#include "LinjeSensor.h"
#include "init.h"


// Line sensors live here


const uint16_t maxSpeed = 150;
int16_t lastError = 0;
int16_t lastSpeedDifference = 0;
uint16_t crossingCount = 0;    // Track total crossings detected

// PID gains - justert for krappere svinger
const float Kp = 1.8 / 14.0;
const float Kd = 1.1 / 14.0;
const float filterAlpha = 0.8; // Mindre treghet i responsen

// Sensor buffer (5 sensors)
const uint8_t numSensors = 5;
uint16_t lineSensorValues[numSensors];

const uint16_t intersectionThreshold = 600;
const uint16_t centeredThreshold = 650;
bool atIntersectionNow = false;
bool intersectionLatched = false;
bool intersectionEvent = false;

static int16_t updateLineState() {
  int16_t position = lineSensors.readLine(lineSensorValues);

  uint8_t activeCount = 0;
  for (uint8_t i = 0; i < numSensors; i++) {
    if (lineSensorValues[i] > intersectionThreshold) {
      activeCount++;
    }
  }

  atIntersectionNow = (activeCount >= 4) ||
                      (lineSensorValues[0] > intersectionThreshold && lineSensorValues[4] > intersectionThreshold);

  if (atIntersectionNow && !intersectionLatched) {
    crossingCount++;
    intersectionEvent = true;
  }
  intersectionLatched = atIntersectionNow;

  return position;
}



// Initialize sensors (call from setup())
void initLineSensors() {
  lineSensors.initFiveSensors();
}

// Calibrate automatically at startup (call from setup())
void calibrateSensors() {
  delay(1000);
  for(uint16_t i = 0; i < 120; i++)
  {
    if (i > 30 && i <= 90)
    {
      motors.setSpeeds(-200, 200);
    }
    else
    {
      motors.setSpeeds(200, -200);
    }

    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);
}




MotorSpeeds lineFollower() {
  MotorSpeeds speeds;
  int16_t position = updateLineState();
  int16_t error = position - 2000;

  // Failsafe fjernet: PID brukes alltid, også hvis alle sensorer ser hvitt

  // PID calculation with separate P and D terms
  int16_t speedDifference = (int16_t)(Kp * error + Kd * (error - lastError));

  // Low-pass filter for smooth transitions
  speedDifference = (int16_t)(filterAlpha * speedDifference + (1 - filterAlpha) * lastSpeedDifference);

  // Storre differanse gir krappere svinger, inkludert mulighet for ett hjul bakover.
  if (speedDifference > 220) speedDifference = 220;
  if (speedDifference < -220) speedDifference = -220;

  lastError = error;
  lastSpeedDifference = speedDifference;

  int16_t leftSpeed = (int16_t)maxSpeed + speedDifference;
  int16_t rightSpeed = (int16_t)maxSpeed - speedDifference;

  // Clamp speeds to valid range
  if (leftSpeed > 300) leftSpeed = 300;
  if (leftSpeed < -300) leftSpeed = -300;
  if (rightSpeed > 300) rightSpeed = 300;
  if (rightSpeed < -300) rightSpeed = -300;

  speeds.left = leftSpeed;
  speeds.right = rightSpeed;
  return speeds;
}

bool isAtIntersection() {
  updateLineState();
  return atIntersectionNow;
}

bool consumeIntersectionEvent() {
  updateLineState();
  bool hadEvent = intersectionEvent;
  intersectionEvent = false;
  return hadEvent;
}

bool isCenteredOnLine() {
  updateLineState();

  bool centerActive = lineSensorValues[2] > centeredThreshold;
  bool leftOuterActive = lineSensorValues[0] > intersectionThreshold;
  bool rightOuterActive = lineSensorValues[4] > intersectionThreshold;

  // Sentrert på en enkel linje: midtsensor ser tydelig linje, og vi står ikke i kryss.
  return centerActive && !leftOuterActive && !rightOuterActive && !atIntersectionNow;
}