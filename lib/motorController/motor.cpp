#include "motor.h"
#include "LinjeSensor.h"
#include "ProxSensors.h"
#include "init.h"

int leftSpeed = 0;
int rightSpeed = 0;

void setSpeeds(){
    
    if (stopNow){
        rightSpeed = leftSpeed = 0;
    }
    
    else{
            MotorSpeeds s = lineFollower();
            rightSpeed = s.right;
            leftSpeed = s.left;
    }

    motors.setSpeeds(rightSpeed, leftSpeed);

}