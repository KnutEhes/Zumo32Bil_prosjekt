// LinjeSensor.h
#pragma once
#include <Arduino.h> // VIKTIG: Gir tilgang til int16_t, uint8_t, osv.

// Struct for å returnere hastighet til begge motorer samtidig
struct MotorSpeeds {
  int16_t left;
  int16_t right;
};

// Funksjonsprototyper
void initLineSensors();
void calibrateSensors();

MotorSpeeds lineFollower();

// True hvis sensorene akkurat nå ser et kryss.
bool isAtIntersection();

// True kun en gang per nytt kryss (edge-trigger), så kan kalles i loop.
bool consumeIntersectionEvent();

// True når bilen ligger sentrert på én linje (nyttig for å avslutte en sving).
bool isCenteredOnLine();

