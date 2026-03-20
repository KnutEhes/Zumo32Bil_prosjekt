#include "Communication.h"
#include "zumoBattery.h"
#define zumoAddress 0x55


zumoInstructions instruksjoner{false, 'r', 'x'};

//Sender info om bilen til ESP ved beskjed
void onSendRequest(){
    Wire.write((uint8_t*)&minIsbil, sizeof(minIsbil));
}


//Mottar instruksjoner ved beskjed fra ESP
void onReceiveRequest(int numBytes){
    if (numBytes == sizeof(instruksjoner)){

        while(Wire.available()){
            Wire.readBytes((byte*)&instruksjoner, sizeof(instruksjoner));

        }
            
    }
    else{
        Serial.println("Størrelser stemmer ikke overens...");
    }
    
    //Til feilsøking, bare å fjerne
    Serial.println();
    Serial.print("grønlys?: ");
    Serial.println(instruksjoner.trafficLightGreen);
    Serial.print("retning på neste schwuing: ");
    Serial.println(instruksjoner.nextTurn);
    Serial.print("Gate: ");
    Serial.println(instruksjoner.posName);
}

//Setup, starter opp og velger funksjoner for callback send/motta
void commSetup(){
    Wire.begin(zumoAddress);
    Wire.onRequest(onSendRequest);
    Wire.onReceive(onReceiveRequest);

}