#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// ─── Konfig ───────────────────────────────────────────
#define WIFI_SSID  "KNUTS"
#define WIFI_PASS  "12345678"
#define WS_HOST    "192.168.137.1"
#define WS_PORT    8765
#define WS_PATH    "/"

// ─── Structs ──────────────────────────────────────────
struct ZumoInstructions {
    bool trafficLightState;
    char nextTurn;
    ZumoInstructions(uint8_t t = 0, char n = 'F')
        : trafficLightState(t), nextTurn(n) {}
};

struct Isbil {
    float balance;
    float speed;
    float batteryLevel;
    uint16_t iceCreams;
    Isbil(float b = 0, float s = 0, float battery = 0, int i = 0)
        : balance(b), speed(s), batteryLevel(battery), iceCreams(i) {}
};

// ─── Node MAC-adresser ────────────────────────────────
uint8_t nodeMacs[][6] = {
    {0x84, 0x1F, 0xE8, 0x39, 0xD5, 0x94},  // node 1 - isbil
    // {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF},  // node 2
};
const int NODE_COUNT = 1;

WebSocketsClient ws;
esp_now_peer_info_t peerInfo;

// ─── Sendt til node: bekreftelse ─────────────────────
void onDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        Serial.println("ESP-NOW til node: OK");
    } else {
        Serial.println("ESP-NOW til node: FEIL");
    }
}

// ─── Mottatt fra node: Isbil struct → JSON → server ──
void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
    if (len < sizeof(Isbil)) {
        Serial.println("Mottatt data er for kort");
        return;
    }

    Isbil isbil;
    memcpy(&isbil, data, sizeof(Isbil));

    Serial.printf("Mottatt fra node — balance: %.2f, speed: %.2f, battery: %.2f, iceCreams: %d\n",
        isbil.balance, isbil.speed, isbil.batteryLevel, isbil.iceCreams);

    // Pakk om til JSON og send til server
    StaticJsonDocument<200> doc;
    doc["balance"]      = isbil.balance;
    doc["speed"]        = isbil.speed;
    doc["batteryLevel"] = isbil.batteryLevel;
    doc["iceCreams"]    = isbil.iceCreams;
    doc["timestamp"]    = millis();

    char jsonBuf[200];
    serializeJson(doc, jsonBuf);

    Serial.printf("Sender JSON: %s\n", jsonBuf);
    ws.sendTXT(jsonBuf);
}

// ─── Mottatt fra server: JSON → ZumoInstructions → node ──
void onWsEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED:
            Serial.println("WS tilkoblet til server");
            break;

        case WStype_TEXT: {
            Serial.printf("Fra server: %s\n", payload);

            StaticJsonDocument<200> doc;
            DeserializationError err = deserializeJson(doc, payload);
            if (err) {
                Serial.println("JSON parse feil");
                break;
            }

            // Bygg ZumoInstructions struct
            ZumoInstructions instr;
            instr.trafficLightState = doc["trafficLightState"] | false;
            const char *turn = doc["nextTurn"] | "F";
            instr.nextTurn = turn[0];

            Serial.printf("Sender til node — trafficLight: %d, nextTurn: %c\n",
                instr.trafficLightState, instr.nextTurn);

            int receiverId = doc["receiverId"] | 0;

            if (receiverId == 0) {
                // Send til alle noder
                for (int i = 0; i < NODE_COUNT; i++) {
                    esp_now_send(nodeMacs[i], (uint8_t*)&instr, sizeof(ZumoInstructions));
                }
            } else if (receiverId >= 1 && receiverId <= NODE_COUNT) {
                // Send til spesifikk node
                esp_now_send(nodeMacs[receiverId - 1], (uint8_t*)&instr, sizeof(ZumoInstructions));
                Serial.printf("Sendt til node %d\n", receiverId);
            } else {
                Serial.printf("Ukjent node ID: %d\n", receiverId);
            }
            break;
        }

        case WStype_DISCONNECTED:
            Serial.println("WS frakoblet, prøver igjen...");
            break;

        default:
            break;
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Kobler til WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\nTilkoblet. IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Bridge MAC: %s\n", WiFi.macAddress().c_str());

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init feilet");
        return;
    }
    esp_now_register_send_cb(onDataSent);
    esp_now_register_recv_cb(onDataRecv);

    for (int i = 0; i < NODE_COUNT; i++) {
        memcpy(peerInfo.peer_addr, nodeMacs[i], 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;
        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
            Serial.printf("Klarte ikke legge til node %d\n", i + 1);
        } else {
            Serial.printf("Node %d registrert\n", i + 1);
        }
    }

    ws.begin(WS_HOST, WS_PORT, WS_PATH);
    ws.onEvent(onWsEvent);
    ws.setReconnectInterval(3000);
}

void loop() {
    ws.loop();
}
