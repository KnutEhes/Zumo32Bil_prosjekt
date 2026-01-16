#include <Cream.h>
#include <buzzAndLed.h>
#include <swBatteri.h>

//barn i nærheten
int childrenNearby = 0;
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
unsigned long kidCheckWaitTime = 10000;




//Ser etter barn og selger is om det er mange nok i nærheten
void iceCream(){

    /*
    ----------Deteksjon av barn-----------
    Genererer antall barn i nabolaget*/
    if (lastKidCheck + kidCheckWaitTime < millis()){
    childrenNearby = random(0, 10);
    Serial.print("Antall barn: ");
    Serial.println(childrenNearby);
    lastKidCheck = millis();
    }

    //Spiller iskbilmusikk når den leter etter barn
    //TRENGER BEDRE LØSNING
    if ((sellingIceCream == false) & (lastIceCreamSold + iceCreamSellTime < millis())){
      iceCreamBuzz();
    }


    //STARTER Å SELGE IS 
    if ((childrenNearby >= childrenNeeded) & (sellingIceCream == false)){
        //Stopp bilen her
        sellingIceCream = true;
        Serial.println("Begynner å selge is");
        lastIceCreamSold = millis();
    }
    //SELGER IS
    if (sellingIceCream == true){
        if (lastIceCreamSold + iceCreamSellTime < millis()){
            //SISTE SALG
            if (childrenNearby < 2){
                childrenNearby = 0;
                sellingIceCream = false;
                //start bilen igjen her
            }
            else{
                childrenNearby -= 1;
            }
            
            iceCreamSellTime = random(1000, 3000);
            balance += random (20, 40);
            kaChingBuzz();
            lastIceCreamSold = millis();
            lastKidCheck = millis();
            Serial.print("Penger totalt: ");
            Serial.println(balance);
        }
    }
}