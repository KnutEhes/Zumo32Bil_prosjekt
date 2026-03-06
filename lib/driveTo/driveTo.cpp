#include "motor.h"
#include "driveTo.h"
#include "LinjeSensor.h"
#include <Arduino.h>
#include <Zumo32U4.h>
float feilMargin = 0.01; //I prosent
unsigned long start; 
int turnLeftSpeed = 0;
int turnRightSpeed = 0;
unsigned long turnTimeout = 0;
unsigned long turnMinDuration = 0;
bool turnInProgress = false;
bool turnLeftIntersection = false;


bool driveTo(const pos& bil, const pos& point){
    // Sjekk om bilen er på riktig punkt (innenfor feilmargin på både x og y)
    bool påRiktigX = (point.x * (1 - feilMargin) < bil.x) && (bil.x < point.x * (1 + feilMargin));
    bool påRiktigY = (point.y * (1 - feilMargin) < bil.y) && (bil.y < point.y * (1 + feilMargin));

    if (påRiktigX && påRiktigY) {
        motors.setSpeeds(0, 0);
        start = 0;
        turnInProgress = false;
        return true;
    }

    // Fullfor påbegynt sving/snu basert på linjesensor: forlat kryss, finn sentrert linje.
    if (turnInProgress) {
        motors.setSpeeds(turnLeftSpeed, turnRightSpeed);

        // Ikke avbryt svingen for tidlig selv om linje oppdages med en gang.
        if (millis() - start < turnMinDuration) {
            return false;
        }

        if (!turnLeftIntersection) {
            if (!isAtIntersection()) {
                turnLeftIntersection = true;
            }
        } else {
            if (isCenteredOnLine()) {
                turnInProgress = false;
                turnLeftIntersection = false;
                start = 0;
                setSpeeds();
                return false;
            }
        }

        // Sikkerhetsnett hvis linje ikke finnes av en eller annen grunn.
        if (millis() - start > turnTimeout) {
            turnInProgress = false;
            turnLeftIntersection = false;
            start = 0;
            setSpeeds();
        }
        return false;
    }

    bool intersectionEvent = consumeIntersectionEvent();

    // Kjør mot riktig x-posisjon først
    if (!påRiktigX) {
        if (bil.x < point.x) {
            start = 0;
            setSpeeds();
        } else {
            // Forbi x: snu kun i kryss.
            if (intersectionEvent || isAtIntersection()) {
                start = millis();
                turnTimeout = 900;
                turnMinDuration = 450;
                turnLeftSpeed = -200;
                turnRightSpeed = 200;
                turnInProgress = true;
                turnLeftIntersection = false;
                motors.setSpeeds(turnLeftSpeed, turnRightSpeed);
            } else {
                setSpeeds();
            }
        }
        return false;
    }

    // Når x er riktig men y er feil: sving opp/ned med timet vridning
    if (!påRiktigY) {
        if (intersectionEvent || isAtIntersection()) {
            start = millis();
            turnTimeout = 700;
            turnMinDuration = 180;
            if (bil.y < point.y) {
                turnLeftSpeed = -200;
                turnRightSpeed = 200;
            } else {
                turnLeftSpeed = 200;
                turnRightSpeed = -200;
            }
            turnInProgress = true;
            turnLeftIntersection = false;
            motors.setSpeeds(turnLeftSpeed, turnRightSpeed);
        } else {
            setSpeeds();
        }
        return false;
    }

    return false;
}


void checkTypePoint(pos point){
    if(point.type == 'F'){ 
    
    } 
    if(point.type == 'S'){

    }
    if(point.type == 'B'){

    }
}