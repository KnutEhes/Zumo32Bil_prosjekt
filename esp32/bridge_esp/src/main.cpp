#include <esp_now.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

WebSocketsClient webSocket;

// 1. MAC-ADRESSE TIL MOTTAKER
uint8_t broadcastAddress[] = {0x84, 0x1F, 0xE8, 0x3A, 0x44, 0x28};

// Strukturer
typedef struct send_message {
    bool trafficLightGreen;
    char nextTurn;
    char posName;
} send_message;

typedef struct receive_message {
    float balance;
    float speed;
    float batteryLevel;
    uint16_t iceCreams;
} receive_message;

send_message infoToSend;
receive_message incomingInfo;
esp_now_peer_info_t peerInfo;

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

// Callback når data er sendt
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback når data er mottatt (Fikset for eldre/standard versjon)
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingInfo, incomingData, sizeof(incomingInfo));
  
  // Vi lager en tekststreng av dataene for å sende sikkert til RPi
  String wsPayload = String(incomingInfo.balance) + "," + 
                     String(incomingInfo.speed) + "," + 
                     String(incomingInfo.batteryLevel) + "," + 
                     String(incomingInfo.iceCreams);
  
  webSocket.sendTXT(wsPayload); // Sender som tekst til Raspberry Pi
  Serial.println("Data videresendt til RPi: " + wsPayload);
}

void setup() {
  Serial.begin(115200);

  // WiFi må settes opp før ESP-NOW
  WiFi.mode(WIFI_STA);
  WiFi.begin("Ditt_SSID", "Ditt_Passord");
  
  Serial.print("Kobler til WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi tilkoblet!");

  // Initialiser ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Registrer callbacks - fjernet unødvendig typecasting som ofte feiler
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  
  // Legg til peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0; // Bruker samme kanal som WiFi
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
  // WebSocket setup (husk å endre IP!)
  webSocket.begin("192.168.x.x", 81, "/"); 
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000); // Prøv på nytt hvert 5. sek hvis den faller ut
}

void loop() {
  webSocket.loop();

  // Oppdater data som skal sendes
  infoToSend.trafficLightGreen = false;
  infoToSend.nextTurn = 'L';
  infoToSend.posName = 'H';

  // Send hvert 5. sekund for å ikke floode nettverket mens du tester
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 5000) {
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &infoToSend, sizeof(infoToSend));
    lastSend = millis();
    
    if (result == ESP_OK) {
      Serial.println("ESP-NOW sendt suksessfullt");
    } else {
      Serial.println("ESP-NOW send feilet");
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