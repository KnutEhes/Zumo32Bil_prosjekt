// Kode for Lyskrysset sin esp

#include <esp_now.h>
#include <WiFi.h>

#define RED 25
#define YELLOW 26
#define GREEN 27

typedef struct {
  int lys;
} message;

message incomingData;

void onReceive(const uint8_t *mac, const uint8_t *data, int len) {

  memcpy(&incomingData, data, sizeof(incomingData));

  digitalWrite(RED, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(GREEN, LOW);

  if (incomingData.lys == 1) {
    digitalWrite(GREEN, HIGH);
  }

  if (incomingData.lys == 2) {
    digitalWrite(YELLOW, HIGH);
  }

  if (incomingData.lys == 3) {
    digitalWrite(RED, HIGH);
  }

}

void setup() {

  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW error");
    return;
  }

  esp_now_register_recv_cb(onReceive);

}

void loop() {
}
