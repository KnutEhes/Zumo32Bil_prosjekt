#ifndef __EVCHARGE_H__
#define __EVCHARGE_H__

#include <Zumo32U4.h>
#include <Arduino.h>

//Define her

extern unsigned long chargeCoolDownValue;
extern unsigned long chargeCoolDownTime;
extern bool chargerPresent;

void proximitySense(void);

#endif
