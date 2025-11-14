#include "hovedModul.h"

Zumo32U4ProximitySensors proxSensors;
Zumo32U4Encoders encoders;


void initZumo(){
    //initialiserer bilen i setup
    proxSensors.initFrontSensor();
    encoders.init();
}

void readSensorValues(){

    //Leser tiden med millis()
    unsigned long time = millis();

    /*
    //Leser avstand sensorer
    proxSensors.read();
    uint8_t leftValue = proxSensors.countsFrontWithLeftLeds();
    uint8_t rightValue = proxSensors.countsFrontWithRightLeds();

    //Leser enkoder verdier
    uint16_t leftEncoderVal = encoders.getCountsLeft();
    uint16_t rightEncoder = encoders.getCountsRight();
    */
}

Motion calculateMotion() {
    Motion m;

    float distanceL = (encoders.getCountsAndResetLeft()  / CPR) * wheelCircumference;
    float distanceR = (encoders.getCountsAndResetRight() / CPR) * wheelCircumference;
    float distStep = 0.5 * (distanceL + distanceR);

    static float totalDistance = 0.0;  
    static float prevSpeed = 0.0;

    totalDistance += distStep;

    m.distance = totalDistance;
    m.speed = distStep;
    m.acceleration = m.speed - prevSpeed;

    prevSpeed = m.speed;

    return m;
}
