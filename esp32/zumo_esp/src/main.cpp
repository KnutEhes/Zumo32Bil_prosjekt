#include <Wire.h>

#define zumoAddress 0x55


struct ZumoInstructions{    //foreløpig struct, ikke i bruk enda.
  uint8_t trafficLightState;
  char nextTurn;
};

struct Isbil {  //totalt 16 bytes av en eller anna grunn
    float balance;  //4bytes
    float speed;    //4bytes
    float batteryLevel; //4bytes
    uint16_t iceCreams;  //2 bytes

    // Fix: Add parameters to the constructor so it actually sets values
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
  // put your main code here, to run repeatedly:
  Wire.requestFrom(zumoAddress, sizeof(minIsbil));
  Wire.readBytes((uint8_t*)&minIsbil, sizeof(Isbil));


  delay(1000);
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
