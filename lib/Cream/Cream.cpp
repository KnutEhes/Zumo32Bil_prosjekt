#include <Cream.h>
#include <buzzAndLed.h>
#include <swBatteri.h>
#include <ProxSensors.h>


//antall barn som må til før vi begynner å selge is
int childrenNeeded = 3;

//tid før neste salg
unsigned long iceCreamSellTime = 5000;
//tidspunkt ved forrige salg
unsigned long lastIceCreamSold = 0;
//Bestemmer om bilen går i salgsmodus
bool sellingIceCream = false;

//Verdier til barnesjekk
unsigned long lastKidCheck = 0;
unsigned long kidCheckWaitTime = 100;

//Verdier til midlertidig posisjonsplassering
unsigned long newPosTime = 0;
int currentPos[2]{0, 0};

//Matrise som viser antall barn i de forskjellig gatene
int kidMatrix[7][4]{
    /*
    {0, 0, 0, 0}, 
    {0, 0, 0, 0}, 
    {0, 0, 0, 0}, 
    {0, 0, 0, 0}, 
    {0, 0, 0, 0}, 
    {0, 0, 0, 0}, 
    {0, 0, 0, 0}, 
    */
};
void creamSetup(){
                for (int i=0; i<7; i++){
                    for (int j=0; j<4; j++){
                        kidMatrix[i][j] = random (0, 3);
                        }

                }
}


//Ser etter barn og selger is om det er mange nok i nærheten
void iceCream(){
    if(!fastCharging & !slowCharging){

        //Midlertidig kode som genererer ny posisjon med antall barn
        if ((sellingIceCream == false) & (millis() > newPosTime+5000)){
            currentPos[0] = random(0, 6);
            currentPos[1] = random(0, 3);
            for (int i=0; i<7; i++){
                Serial.println();
                for (int j=0; j<4; j++){
                    Serial.print(kidMatrix[i][j]);
                    }

                }
            Serial.println();
            Serial.print("Posisjon : ");
            Serial.print(currentPos[0]);
            Serial.println(currentPos[1]);
            Serial.print("Antall barn: ");
            Serial.println(kidMatrix[currentPos[0]][currentPos[1]]);
            newPosTime = millis();
        }
        /*
        ----------Deteksjon av barn-----------
        Foreløpig fungerende kode
        Genererer antall barn i hele nabolaget*/

        if (millis() > (lastKidCheck + kidCheckWaitTime)){
            for (int i=0; i<7; i++){
                for (int j=0; j<4; j++){
                    int terning = random(0, 1000);
                    if ((terning < 3) & (kidMatrix[i][j] < 9)){
                        kidMatrix[i][j] +=1;
                    }
                    if ((terning > 998) & (kidMatrix[i][j] > 0)){
                        kidMatrix[i][j] -=1;
                    }

                }
                
            }
            lastKidCheck = millis();
        }

        //Spiller isbilmusikk når den leter etter barn
        //TRENGER BEDRE LØSNING
        if ((sellingIceCream == false) & (lastIceCreamSold + iceCreamSellTime < millis())){
        //iceCreamBuzz();
        }


        //STARTER Å SELGE IS 
        if ((kidMatrix[currentPos[0]][currentPos[1]] >= childrenNeeded) & (sellingIceCream == false)){
            sellingIceCream = true;
            Serial.print("Begynner å selge is for følgende antall barn: ");
            Serial.println(kidMatrix[currentPos[0]][currentPos[1]]);
            lastIceCreamSold = millis();
        }
        //SELGER IS
        if (sellingIceCream == true){
            stopNow = true; //stopper bilen
            if (lastIceCreamSold + iceCreamSellTime < millis()){
                //SISTE SALG
                if (kidMatrix[currentPos[0]][currentPos[1]] < 2){
                    kidMatrix[currentPos[0]][currentPos[1]] = 0;
                    sellingIceCream = false;
                    stopNow = false;    //starter bilen igjen
                }
                else{
                    kidMatrix[currentPos[0]][currentPos[1]] -= 1;
                }
                
                iceCreamSellTime = random(1000, 3000);
                balance += random (20, 40);
                //kaChingBuzz();
                lastIceCreamSold = millis();
                lastKidCheck = millis();
                Serial.print("Penger totalt: ");
                Serial.println(balance);
            }
        }
    }
}