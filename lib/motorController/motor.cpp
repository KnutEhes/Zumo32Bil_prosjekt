#include "motor.h"
#include "LinjeSensor.h"
#include "ProxSensors.h"
#include "init.h"
#include "zumoBattery.h"
#include "Cream.h"

int leftSpeed = 0;
int rightSpeed = 0;

void setSpeeds(){
    
    if (proximitySense() ||(zumoBatteri.getPercentage() == 0)){ //Bilen skal stoppe hvis noe av dette er sant
        rightSpeed = leftSpeed = 0;
    }
    
    else{
            MotorSpeeds s = lineFollower();
            rightSpeed = s.right;
            leftSpeed = s.left;
    }
    minIsbil.speed = ((leftSpeed + rightSpeed) / 2);
    motors.setSpeeds(leftSpeed, rightSpeed);

}