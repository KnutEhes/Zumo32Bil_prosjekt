// LinjeSensor.h
#pragma once
#include <Arduino.h> // VIKTIG: Gir tilgang til int16_t, uint8_t, osv.

// Struct for å returnere hastighet til begge motorer samtidig
struct MotorSpeeds {
  int16_t left;
  int16_t right;
};

// Struct for alle sensordata samlet
struct LinjesensorData {
  uint16_t sensorVerdi[5];    // Endret til uint16_t (standard for Zumo-sensorer)
  bool kryss;
  int16_t kryss_posisjon;     // Posisjon i krysset (-2000 til 2000)
  uint16_t kryss_count;       // Totalt antall kryss detektert
  uint8_t grener;             // Antall grener (2, 3 eller 4)
};

// Funksjonsprototyper
void initLineSensors();
void calibrateSensors();
LinjesensorData getLineSensorData();
MotorSpeeds lineFollower();

