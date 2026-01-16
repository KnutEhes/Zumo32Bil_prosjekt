// PID.cpp  — black line on white background
#include <Arduino.h>
#include <Zumo32U4.h>
#include "PID.h"



// Line sensors live here
static Zumo32U4LineSensors lineSensors;
extern Zumo32U4Motors motors;

const uint16_t maxSpeed = 200;
int16_t lastError = 0;
int16_t lastSpeedDifference = 0;

// PID gains - tuned for smooth turning with sharp turn capability
const float Kp = 1.0 / 14.0;  // Proportional gain - balanced for sharp turns and stability
const float Kd = 0.8 / 14.0;  // Derivative gain - increased for better damping
const float filterAlpha = 0.55; // Low-pass filter - higher for smoother transitions

// Sensor buffer (5 sensors)
static const uint8_t numSensors = 5;
static uint16_t lineSensorValues[numSensors];



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
  int16_t position = lineSensors.readLine(lineSensorValues);
  int16_t error = position - 2000;

  // PID calculation with separate P and D terms
  int16_t speedDifference = (int16_t)(Kp * error + Kd * (error - lastError));
  
  // Low-pass filter for smooth transitions
  speedDifference = (int16_t)(filterAlpha * speedDifference + (1 - filterAlpha) * lastSpeedDifference);
  
  // Limit speed difference to allow sharp turns but prevent extremes
  if (speedDifference > 150) speedDifference = 150;
  if (speedDifference < -150) speedDifference = -150;

  lastError = error;
  lastSpeedDifference = speedDifference;

  int16_t leftSpeed = (int16_t)maxSpeed + speedDifference;
  int16_t rightSpeed = (int16_t)maxSpeed - speedDifference;

  // Clamp speeds to valid range
  if (leftSpeed > 400) leftSpeed = 400;
  if (leftSpeed < -400) leftSpeed = -400;
  if (rightSpeed > 400) rightSpeed = 400;
  if (rightSpeed < -400) rightSpeed = -400;

  speeds.left = leftSpeed;
  speeds.right = rightSpeed;
  return speeds;
}
