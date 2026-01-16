#include <init.h>
#include "PID.h"

Zumo32U4ProximitySensors proxSensor;

Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Buzzer buzzer;

Zumo32U4OLED display;
Zumo32U4LineSensors lineSensors;

Zumo32U4Motors motors;


void initAll(){

    Serial.begin(9600);
    Serial.println("hello");
    proxSensor.initFrontSensor();
    display.init();
    lineSensors.initFiveSensors();
    Serial.begin(9600);
    delay(20);
    calibrateSensors();

}
