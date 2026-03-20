#include <Arduino.h>
#include <Wire.h>

#define zumoAddress 0x55  //adresse til zumobilen

unsigned long lastI2CTransmit = 50;  //offsetter så receive og transmit ikke er samtidig.
unsigned long lastI2CReceive = 0;
unsigned long I2CTxRxWaitTime = 100;  //Tid mellom send/motta data


struct zumoInstructions { //Struct for instruksjoner MÅ SAMSVARE MED bil
    bool trafficLightGreen;
    char nextTurn;
    char posName;

    zumoInstructions(bool l = true, char t = 0, char p = 0)
        : trafficLightGreen(l), nextTurn(t), posName(p) {}
};

zumoInstructions instruksjoner{true, 'l', 'a'};


struct Isbil {  //totalt 16 bytes av en eller anna grunn, må samsvare denne og
    float balance;  //4bytes
    float speed;    //4bytes
    float batteryLevel; //4bytes
    uint16_t iceCreams;  //2 bytes


    Isbil(float b = 0, float s = 0, float bL = 0, uint16_t i = 0) 
        : balance(b),  speed(s), batteryLevel(bL), iceCreams(i) {}
};


Isbil minIsbil{1000.0, 0.0, 10, 50};

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);

}

void loop() {
  
  //Ber om data fra Zumobilen
  if(lastI2CReceive < millis()-I2CTxRxWaitTime){
    Wire.requestFrom(zumoAddress, sizeof(minIsbil));
    Wire.readBytes((uint8_t*)&minIsbil, sizeof(Isbil));
    lastI2CReceive = millis();

    //til feilsøking, bare å fjerne
    Serial.println();
    Serial.print("størrelse på paketet: ");
    Serial.println(sizeof(minIsbil));
    Serial.print("balance: ");
    Serial.println(minIsbil.balance);
    Serial.print("fart (vroooom): ");
    Serial.println(minIsbil.speed);
    Serial.print("batterinivå: ");
    Serial.println(minIsbil.batteryLevel);
    Serial.print("antall is i bagasjen: ");
    Serial.println(minIsbil.iceCreams);
  }

  //Sender instruksjoner til bilen
  if(lastI2CTransmit < millis()-I2CTxRxWaitTime){
    Wire.beginTransmission(zumoAddress);
    Wire.write((uint8_t*)&instruksjoner, sizeof(instruksjoner));
    Wire.endTransmission();

    lastI2CTransmit = millis();
  }

  

}
