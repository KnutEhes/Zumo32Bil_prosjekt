//
// Created by knuth on 14.11.2025.
//

#ifndef PID_H
#define PID_H

#include "Zumo32U4.h"
#include "Wire.h"
#include "Arduino.h"

// Struct for returning both motor speeds
struct MotorSpeeds {
  int16_t left;
  int16_t right;
};

// These are just declarations (prototypes)
MotorSpeeds lineFollower();

#endif //PID_H
