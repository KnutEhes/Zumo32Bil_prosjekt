//
// Created by knuth on 14.11.2025.
//

#ifndef PID_H
#define PID_H

// Struct for returning both motor speeds
struct MotorSpeeds {
  int16_t left;
  int16_t right;
};

struct LinjesensorData{
  int16_t sensorVerdi[5];
  bool kryss;
  int16_t kryss_posisjon;      // Position of crossing (-2000 to 2000, 0 = center)
  uint16_t kryss_count;        // Number of crossings detected
  uint8_t grener;              // Number of branches detected (2-4)
};

// These are just declarations (prototypes)
void initLineSensors();
void calibrateSensors();

LinjesensorData getLineSensorData();
MotorSpeeds lineFollower();

#endif //PID_H
