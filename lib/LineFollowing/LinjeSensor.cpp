// PID.cpp  — black line on white background
#include <Arduino.h>
#include <Zumo32U4.h>
#include "PID.h"



// Line sensors live here
#include "init.h"

const uint16_t maxSpeed = 200;
int16_t lastError = 0;
int16_t lastSpeedDifference = 0;
uint16_t crossingCount = 0;    // Track total crossings detected

// PID gains - tuned for smooth turning with sharp turn capability
const float Kp = 1.0 / 14.0;  // Proportional gain - balanced for sharp turns and stability
const float Kd = 0.8 / 14.0;  // Derivative gain - increased for better damping
const float filterAlpha = 0.55; // Low-pass filter - higher for smoother transitions

// Sensor buffer (5 sensors)
const uint8_t numSensors = 5;
uint16_t lineSensorValues[numSensors];



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


// Get line sensor data with cross detection
LinjesensorData getLineSensorData() {
  LinjesensorData data;
  int16_t position = lineSensors.readLine(lineSensorValues);
  
  // Copy sensor values to struct
  for(uint8_t i = 0; i < 5; i++) {
    data.sensorVerdi[i] = lineSensorValues[i];
  }
  
  // Threshold for detecting black line
  const uint16_t threshold = 500;
  
  // Count how many sensors see black
  uint8_t sensorCount = 0;
  bool left = lineSensorValues[0] > threshold;    // Index 0
  bool center_left = lineSensorValues[1] > threshold;  // Index 1
  bool center = lineSensorValues[2] > threshold;  // Index 2 (center)
  bool center_right = lineSensorValues[3] > threshold; // Index 3
  bool right = lineSensorValues[4] > threshold;   // Index 4
  
  if (left) sensorCount++;
  if (center_left) sensorCount++;
  if (center) sensorCount++;
  if (center_right) sensorCount++;
  if (right) sensorCount++;
  
  // Detect crossing pattern
  if (sensorCount >= 3) {  // At least 3 sensors see black = crossing
    data.kryss = true;
    data.kryss_posisjon = position;
    
    // Determine number of branches
    if (center && left && right) {
      // All three outer sensors = 4-way crossing (+ form)
      data.grener = 4;
    } else if ((center && left && center_right) || (center && center_left && right)) {
      // T-junction: center + left+center_left OR center + right+center_right = 3 branches
      data.grener = 3;
    } else {
      // Default: 2-way
      data.grener = 2;
    }
    
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

//Get line sensor data with cross detection
LinjesensorData getLineSensorData() {
  LinjesensorData data;
  int16_t position = lineSensors.readLine(lineSensorValues);
  
  // Copy sensor values to struct
  for(uint8_t i = 0; i < 5; i++) {
    data.sensorVerdi[i] = lineSensorValues[i];
  }



  
  // Detect crossing - if all sensors see black line or center sensors are saturated
  bool allSensors = (lineSensorValues[0] > 500 && lineSensorValues[1] > 500 && 
                     lineSensorValues[2] > 500 && lineSensorValues[3] > 500 && 
                     lineSensorValues[4] > 500);
  data.kryss = allSensors;
  
  return data;
}