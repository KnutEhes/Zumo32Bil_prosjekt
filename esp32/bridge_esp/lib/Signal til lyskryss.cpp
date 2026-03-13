#include <esp_now.h>
#include <WiFi.h>

uint8_t lysESP[] = {0x00,0x4b,0x12,0x3c,0x24,0xc0}; // MAC til lyskryss

typedef struct {
  int lys;
} message;

message msg;

unsigned long forrigeTid = 0;
unsigned long intervall = 5000;

int state = 1; // 1 = grønn, 2 = gul, 3 = rød

void setup() {

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW error");
    return;
  }

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, lysESP, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  esp_now_add_peer(&peerInfo);

  msg.lys = state;
  esp_now_send(lysESP, (uint8_t *)&msg, sizeof(msg));
}

void loop() {

  unsigned long nåTid = millis();

  if (nåTid - forrigeTid >= intervall) {

    forrigeTid = nåTid;

    // bytt lys
    if (state == 1) {       // grønn → gul
      state = 2;
      intervall = 3000;
    }
    else if (state == 2) {  // gul → rød
      state = 3;
      intervall = 5000;
    }
    else if (state == 3) {  // rød → grønn
      state = 1;
      intervall = 5000;
    }

    msg.lys = state;
    esp_now_send(lysESP, (uint8_t *)&msg, sizeof(msg));
  }

}