#include "PID.h"
#include "init.h"

// PID constants (tune these for your robot)
float Kp = 1.0f;
float Ki = 0.01f;
float Kd = 0.5f;

// Base motor speed
const int16_t baseSpeed = 150;

// PID state
int16_t lastError = 0;
long integral = 0;


unsigned int sensorValues[5];



// Initialize sensors (call from setup() in main file)


// Pure line-following function: reads sensors and returns desired speeds
MotorSpeeds lineFollower() {
  MotorSpeeds speeds;

  // 1. Read line position (0..4000, ~2000 = center)
  uint16_t position = lineSensors.readLine(sensorValues);

  // 2. Compute error relative to center
  int16_t error = (int16_t)position - 2000;

  // 3. PID terms
  integral += error;

  // Anti-windup on integral
  if (integral > 10000) integral = 10000;
  if (integral < -10000) integral = -10000;

  int16_t derivative = error - lastError;
  lastError = error;

  // 4. Compute turn amount
  float turn = Kp * error + Ki * integral + Kd * derivative;
  int16_t turn_speed = (int16_t)turn;

  // 5. Compute raw speeds
  int16_t leftSpeed  = baseSpeed - turn_speed;
  int16_t rightSpeed = baseSpeed + turn_speed;

  // 6. Constrain to allowed range (-400..400 so it can turn in place)
  leftSpeed  = constrain(leftSpeed,  -400, 400);
  rightSpeed = constrain(rightSpeed, -400, 400);

  // 7. Fill struct and return
  speeds.left  = leftSpeed;
  speeds.right = rightSpeed;
  return speeds;
}
