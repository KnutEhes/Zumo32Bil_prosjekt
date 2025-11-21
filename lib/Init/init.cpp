#include <init.h>

Zumo32U4ProximitySensors proxSensor;

Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Buzzer buzzer;

Zumo32U4OLED display;

void initAll(){

    Serial.begin(9600);
    proxSensor.initFrontSensor();
    display.init();

}