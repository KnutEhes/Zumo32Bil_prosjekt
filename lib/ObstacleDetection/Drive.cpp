#include <Wire.h>
#include <Zumo32U4.h>
#include "init.h"

// --- Objekter fra Zumo-biblioteket ---

// --- Konfigurasjon line-følger ---
const uint16_t baseSpeed = 200;   // grunnfart fremover (0–400)
const int16_t Kp = 20;            // proporsjonal faktor (juster ved behov)
const int16_t Kd = 60;            // derivativ faktor (juster ved behov)

// lineSensors.readLine() gir verdi 0–4000, midten ~2000
int16_t lastError = 0;

// --- Konfigurasjon for hinder ---
const uint8_t proxThreshold = 3;  // hvor "sterkt" signal før vi sier at noe er foran
const uint16_t obstacleBackupTime = 400; // ms å rygge når hinder oppdages
const uint16_t obstacleTurnTime   = 500; // ms å svinge rundt hinder (prøv deg fram)

// --- Funksjoner ---

void calibrateLineSensors()
{
  display.clear();
  display.print(F("Kalibrer"));
  delay(1000);

  // En enkel kalibreringssekvens der roboten roterer litt frem og tilbake
  for (uint8_t i = 0; i < 80; i++)
  {
    if (i < 20 || i >= 60)
    {
      motors.setSpeeds(150, -150); // venstre
    }
    else
    {
      motors.setSpeeds(-150, 150); // høyre
    }

    lineSensors.calibrate();
    delay(20);
  }

  motors.setSpeeds(0, 0);
  display.clear();
  display.print(F("Ferdig"));
  delay(500);
  display.clear();
}

bool obstacleDetected()
{
  proxSensors.read();
  uint8_t left  = proxSensors.countsFrontWithLeftLeds();
  uint8_t right = proxSensors.countsFrontWithRightLeds();

  bool seen = (left >= proxThreshold) || (right >= proxThreshold);

  display.gotoXY(0, 0);
  display.print(left);
  display.print(' ');
  display.print(right);
  display.print(' ');

  return seen;
}

void avoidObstacle()
{
  display.gotoXY(0, 1);
  display.print(F("HINDER   "));

  // 1. Stopp et øyeblikk
  motors.setSpeeds(0, 0);
  delay(100);

  // 2. Rygge litt rett bakover
  motors.setSpeeds(-200, -200);
  delay(obstacleBackupTime);

  // 3. Sving til høyre en stund for å prøve å komme rundt
  motors.setSpeeds(200, -200);
  delay(obstacleTurnTime);

  // 4. Rull sakte fremover til vi ser linja igjen eller hinderet er borte
  uint16_t lineValues[5];
  unsigned long startTime = millis();
  while (millis() - startTime < 2000)  // maks 2 sekunders forsøk
  {
    // Hvis fortsatt hinder rett foran, fortsett å svinge
    if (obstacleDetected())
    {
      motors.setSpeeds(200, -200);
    }
    else
    {
      // Prøv å finne linja igjen
      lineSensors.read(lineValues);
      // Summér midt-sensorene for å sjekke om vi er på linja
      uint16_t mid = lineValues[2];

      // Juster terskel etter hvor mørk/lys tapen og gulvet er
      if (mid > 500)  // betyr typisk at vi ser linja
      {
        break; // vi antar vi har funnet linja, går tilbake til normalt linjefølge-oppsett
      }

      motors.setSpeeds(150, 150); // rull sakte frem
    }

    delay(20);
  }

  // Stopp kort før vi gir kontrollen tilbake til linjefølgeren
  motors.setSpeeds(0, 0);
  delay(100);
  display.gotoXY(0, 1);
  display.print(F("FOLGER   "));
}

// --- Setup og loop ---

void setup()
{
  lineSensors.initFiveSensors();
  proxSensors.initFrontSensor();

  display.clear();
  display.print(F("Trykk A"));
  buttonA.waitForButton();

  calibrateLineSensors();

  display.clear();
  display.print(F("Starter"));
  delay(500);
  display.clear();
}

void loop()
{
  // Først: sjekk om det er hinder foran
  if (obstacleDetected())
  {
    avoidObstacle();
    return;  // hopp over linjefølger denne runden
  }

  // --- Linjefølger-logikk ---

  uint16_t linePosition = lineSensors.readLine();  // 0–4000
  int16_t error = (int16_t)linePosition - 2000;    // negativ hvis til venstre, positiv hvis til høyre

  int16_t derivative = error - lastError;
  lastError = error;

  int16_t turn = Kp * error / 100 + Kd * derivative / 100;

  int16_t leftSpeed  = (int16_t)baseSpeed + turn;
  int16_t rightSpeed = (int16_t)baseSpeed - turn;

  // Begrens til motorenes område
  leftSpeed  = constrain(leftSpeed, -400, 400);
  rightSpeed = constrain(rightSpeed, -400, 400);

  motors.setSpeeds(leftSpeed, rightSpeed);

  // Litt debug på skjermen
  display.gotoXY(0, 1);
  display.print((int) (linePosition / 10)); // enklere tall
  display.print(' ');
  display.print((int)leftSpeed);
  display.print(' ');
  display.print((int)rightSpeed);
  display.print(' ');
}
