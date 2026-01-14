#include "speedcontroll.h"
#include "init.h"
#include "ProxSensors.h"
#include "PID.h"

void setSpeed(){
    if (stopNow){
        motors.setSpeeds(0, 0);
    }
    else{
        MotorSpeeds s = lineFollower(); 
        motors.setSpeeds(s.left,s.right);
    }
    
  
}