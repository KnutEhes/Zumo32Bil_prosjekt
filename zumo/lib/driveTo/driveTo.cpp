#include "motor.h"
#include "driveTo.h"
#include "LinjeSensor.h"
#include "init.h"
#include <Arduino.h>
#include <Zumo32U4.h>

static unsigned long start = 0;
static int turnLeftSpeed = 0;
static int turnRightSpeed = 0;
static unsigned long turnTimeout = 0;
static unsigned long turnMinDuration = 0;
static bool turnInProgress = false;
static bool turnLeftIntersection = false;

// Returnerer true når svingen er fullført — klar for neste instruksjon fra server.
// nextDir: 'F' = rett frem, 'L' = venstre, 'R' = høyre, 'U' = snu (180°)
bool driveTo(char nextDir) {

    // Fullfør pågående sving
    if (turnInProgress) {
        motors.setSpeeds(turnLeftSpeed, turnRightSpeed);

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
                return true;
            }
        }

        // Sikkerhetsnett hvis linje ikke finnes
        if (millis() - start > turnTimeout) {
            turnInProgress = false;
            turnLeftIntersection = false;
            start = 0;
            setSpeeds();
            return true;
        }
        return false;
    }

    // Kryss oppdaget: utfør retning
    if (consumeIntersectionEvent()) {
        if (nextDir == 'F') {
            setSpeeds();
            return true;
        }

        start = millis();
        turnInProgress = true;
        turnLeftIntersection = false;

        if (nextDir == 'U') {
            turnTimeout = 900;
            turnMinDuration = 450;
            turnLeftSpeed = -200;
            turnRightSpeed = 200;
        } else if (nextDir == 'L') {
            turnTimeout = 700;
            turnMinDuration = 180;
            turnLeftSpeed = -200;
            turnRightSpeed = 200;
        } else { // 'R'
            turnTimeout = 700;
            turnMinDuration = 180;
            turnLeftSpeed = 200;
            turnRightSpeed = -200;
        }

        motors.setSpeeds(turnLeftSpeed, turnRightSpeed);
    } else {
        setSpeeds();
    }

    return false;
}
