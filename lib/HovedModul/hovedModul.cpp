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

    //Leser avstand sensorer
    proxSensors.read();
    uint8_t leftValue = proxSensors.countsFrontWithLeftLeds();
    uint8_t rightValue = proxSensors.countsFrontWithRightLeds();

    //Leser enkoder verdier
    uint16_t leftEncoderVal = encoders.getCountsLeft();
    uint16_t rightEncoder = encoders.getCountsRight();
}