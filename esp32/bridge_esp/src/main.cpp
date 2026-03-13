#include <esp_now.h>
#include <WiFi.h>
// #include <WebSocketsClient.h>

// WebSocketsClient webSocket;

// 1. MAC-ADRESSE TIL MOTTAKER
uint8_t zumoAddress[] = {0x84, 0x1F, 0xE8, 0x3A, 0x44, 0x28};
uint8_t lightAddress[] = {0x00, 0x4b, 0x12, 0x3c, 0x24, 0xc0};

void printIncomingData();

// Strukturer
typedef struct zumo_message {
    bool trafficLightGreen;
    char nextTurn;
    char posName;
} zumo_message;

typedef struct light_message {
    bool isGreen;
} light_message;

typedef struct receive_message {
    float balance;
    float speed;
    float batteryLevel;
    uint16_t iceCreams;
} receive_message;

zumo_message infoToZumo;
light_message infoToLight;
receive_message incomingInfo;
esp_now_peer_info_t peerInfo;

/*
// WebSocket Event Handler
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("[WS] Disconnected!");
      break;
    case WStype_CONNECTED:
      Serial.println("[WS] Connected to RPi!");
      break;
    case WStype_TEXT:
      Serial.printf("[WS] Melding fra RPi: %s\n", payload);
      break;
  }
}
*/

// Callback når data er sendt
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback når data er mottatt
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingInfo, incomingData, sizeof(incomingInfo));
  
  // Serial.println-logikk for debugging
  String debugPayload = String(incomingInfo.balance) + "," + 
                        String(incomingInfo.speed) + "," + 
                        String(incomingInfo.batteryLevel) + "," + 
                        String(incomingInfo.iceCreams);
  
  // webSocket.sendTXT(wsPayload); 
  Serial.println("Mottatt fra Zumo: " + debugPayload);
}

void setup() {
  Serial.begin(115200);

  // WiFi mode må være STA for ESP-NOW
  WiFi.mode(WIFI_STA);
  
  /*
  WiFi.begin("Ditt_SSID", "Ditt_Passord");
  Serial.print("Kobler til WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi tilkoblet!");
  */

  // Initialiser ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  
  // Legg til peer
  memcpy(peerInfo.peer_addr, zumoAddress, 6);
  peerInfo.channel = 0; 
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) Serial.println("Feilet å legge til Zumo");

  // NYTT: Registrer Lyskryss som Peer
  memcpy(peerInfo.peer_addr, lightAddress, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) Serial.println("Feilet å legge til Lyskryss");
  
  /*
  // WebSocket setup
  webSocket.begin("192.168.x.x", 81, "/"); 
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000); 
  */
}

void loop() {
  // webSocket.loop();

  // Oppdater data som skal sendes


  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000) {
    lastSend = millis();

    // 1. OPPSETT AV DATA (Gjør dette først!)
    infoToZumo.trafficLightGreen = false;
    infoToZumo.nextTurn = 'L';
    infoToZumo.posName = 'H';

    infoToLight.isGreen = true; 

    // 2. SEND DATA (Etter at verdiene er satt)
    esp_err_t resultZumo = esp_now_send(zumoAddress, (uint8_t *) &infoToZumo, sizeof(infoToZumo));
    esp_err_t resultLight = esp_now_send(lightAddress, (uint8_t *) &infoToLight, sizeof(infoToLight));
    
    // Debug-utskrift
    if (resultZumo == ESP_OK && resultLight == ESP_OK) {
      Serial.println("Begge meldingene ble sendt!");
    } else {
      Serial.println("En eller begge sendingene feilet.");
    }
    
    printIncomingData();
  }
}

void printIncomingData(){
  Serial.println("--- SISTE DATA MOTTATT ---");
  Serial.print("Balance: "); Serial.println(incomingInfo.balance);
  Serial.print("Speed: "); Serial.println(incomingInfo.speed);
  Serial.print("Battery: "); Serial.print(incomingInfo.batteryLevel); Serial.println("%");
  Serial.print("Ice Creams: "); Serial.println(incomingInfo.iceCreams);
  Serial.println("--------------------------");
}