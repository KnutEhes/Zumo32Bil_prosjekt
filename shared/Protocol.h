struct Isbil {  //totalt 16 bytes av en eller anna grunn
    float balance;  //4bytes
    float speed;    //4bytes
    float batteryLevel; //4bytes
    uint16_t iceCreams;  //2 bytes

    // Fix: Add parameters to the constructor so it actually sets values
    Isbil(float b = 0, float s = 0, float bL = 0, uint16_t i = 0) 
        : balance(b),  speed(s), batteryLevel(bL), iceCreams(i) {}
};


typedef struct recive_message {
    bool trafficLightGreen;
    char nextTurn;
    char posName;

    recive_message(bool TLG, char NT, char PN)
        : trafficLightGreen(TLG), nextTurn(NT), posName(PN) {}

} recive_message;



typedef struct {
    char json[ESPNOW_MAX_LEN];
} EspNowMessage;