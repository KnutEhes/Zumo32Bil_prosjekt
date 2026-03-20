/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <Arduino.h>
#include <Wire.h>

#define zumoAddress 0x55

/*
struct ZumoInstructions{    //foreløpig struct, ikke i bruk enda.
  uint8_t trafficLightState;
  char nextTurn;
};
*/

struct sendMessage {  //totalt 16 bytes av en eller anna grunn
    float balance;  //4bytes
    float speed;    //4bytes
    float batteryLevel; //4bytes
    uint16_t iceCreams;  //2 bytes

    // Fix: Add parameters to the constructor so it actually sets values
    sendMessage(float b = 0, float s = 0, float bL = 0, uint16_t i = 0) 
        : balance(b),  speed(s), batteryLevel(bL), iceCreams(i) {}
};


sendMessage minIsbilInfo{1000.0, 0.0, 10, 50};


void updateDisplay();

// REPLACE WITH THE MAC Address of your receiver 
//uint8_t broadcastAddress[] = {0x64, 0xB7, 0x08, 0x29, 0x1A, 0x2C};
//uint8_t broadcastAddress[] = {0x84, 0x1F, 0xE8, 0x3A, 0x44, 0x28};
uint8_t broadcastAddress[] = {0x00, 0x4B, 0x12, 0x3B, 0x7C, 0x3C};

// Define variables to store BME280 readings to be sent
float balanceReading;
float speedReading;
float batteryLevelReading;
uint16_t iceCreamsReading;

// Define variables to store incoming readings
//bool trafficLightGreen;
//char nextTurn; //Sier hvilken vei du skal svinge i neste sving
//char posName; //Sier om man kjører, er på lader eller selger is osv.

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure

/*
typedef struct send_message {
    float balance;
    float speed;
    float batteryLevel;
    uint16_t iceCreams;
} send_message;
*/



typedef struct recive_message {
    bool trafficLightGreen;
    char nextTurn;
    char posName;
} recive_message;

// Create a struct_message called BME280Readings to hold sensor readings
//send_message IsbilInfo(minIsbil);

// Create a struct_message to hold incoming sensor readings
recive_message incomingInfo;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingInfo, incomingData, sizeof(incomingInfo));
  Serial.print("Bytes received: ");
  Serial.println(len);
  char incomingDir = incomingInfo.nextTurn;
  char incomingPosInfo = incomingInfo.posName;
  bool incomingTLS = incomingInfo.trafficLightGreen;
}
 
void setup() {
  unsigned long int lastSend = millis();
  Serial.begin(115200);
  Wire.begin();
  WiFi.mode(WIFI_STA);
  // Set device as a Wi-Fi Station

  // Hvis ruteren din er på kanal 6, skriv 6 her. 
  // Du kan finne kanalen ved å skrive Serial.println(WiFi.channel()) på Gatewayen.
  int channel = 11; 
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}
 
void loop() {
  lastSend = millis();
 
  Wire.requestFrom(zumoAddress, sizeof(minIsbilInfo));
  Wire.readBytes((uint8_t*)&minIsbilInfo, sizeof(minIsbilInfo));

  esp_err_t result;
  byte available = Wire.requestFrom(zumoAddress, sizeof(minIsbilInfo));
  if (available == sizeof(minIsbilInfo)) {
      Wire.readBytes((uint8_t*)&minIsbilInfo, sizeof(minIsbilInfo));

      result = esp_now_send(broadcastAddress, (uint8_t *) &minIsbilInfo, sizeof(minIsbilInfo));

  }
  


  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  updateDisplay();
  delay(1000);
}


void updateDisplay(){
  
  // Display Readings in Serial Monitor
  Serial.println("INCOMING READINGS");
  Serial.print("Neste sving: ");
  Serial.print(incomingInfo.nextTurn);
  Serial.println(" ");
  Serial.print("Posisjon nå: ");
  Serial.print(incomingInfo.posName);
  Serial.println(" ");
  Serial.print("Neste Trafikklys er grønt: ");
  Serial.print(incomingInfo.trafficLightGreen);
  Serial.println(" ");
  Serial.println();

  Serial.print("størrelse på paketet: ");
  Serial.println(sizeof(minIsbilInfo));
  Serial.print("balance: ");
  Serial.println(minIsbilInfo.balance);
  Serial.print("fart (vroooom): ");
  Serial.println(minIsbilInfo.speed);
  Serial.print("batterinivå: ");
  Serial.println(minIsbilInfo.batteryLevel);
  Serial.print("antall is i bagasjen: ");
  Serial.println(minIsbilInfo.iceCreams);
  
}
