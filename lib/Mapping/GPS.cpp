#include <Wire.h>
#include <Zumo32U4.h>
#include "LinjeSensor.h"

Zumo32U4IMU imu;
char heading = 'N';

// Convert heading degrees (0..360) into N/E/S/W
static char quadrantFromDeg(float deg)
{
  while (deg < 0) deg += 360.0;
  while (deg >= 360.0) deg -= 360.0;

  int q = (int)((deg + 45.0) / 90.0) % 4; // 0..3

  switch (q) {
    case 0: return 'N';
    case 1: return 'E';
    case 2: return 'S';
    default: return 'W';
  }
}

void initCompass()
{
  Wire.begin();

  if (!imu.init()) {
    // IMU not detected
    while (1) {}
  }
  imu.enableDefault();

  delay(200);     // let sensors settle

  // Read magnetometer once
  imu.readMag();

  float mx = (float)imu.m.x;
  float my = (float)imu.m.y;

  // Heading in degrees 0..360
  float stHeading = atan2(my, mx) * 180.0 / PI;
  if (stHeading < 0) stHeading += 360.0;

  heading = quadrantFromDeg(stHeading);
}

char startHeading()
{
  return heading;
}
