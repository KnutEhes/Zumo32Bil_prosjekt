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
};

struct Isbil {
    float balance;
    float speed;
    float batteryLevel;
    uint16_t iceCreams;
};

struct AnchorData {
    float distance;
    float raw;
    float rssi;
    float fp_rssi;
    uint32_t round_time;
    uint32_t reply_time;
    float clock_offset;
};

struct UWBTag {
    uint16_t tag_id;
    AnchorData A1;
    AnchorData A2;
    AnchorData A3;
};

// ─── Node MAC-adresser ────────────────────────────────
uint8_t nodeMacs[][6] = {
    {0x84, 0x1F, 0xE8, 0x39, 0xD5, 0x94},  // node 1 - isbil
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},   // node 2 - UWB (bytt ut med riktig MAC)
};
const int NODE_COUNT = 2;

WebSocketsClient ws;
esp_now_peer_info_t peerInfo;
unsigned long lastNodeRecv[NODE_COUNT] = {0};

// ─── Sendt til node: bekreftelse ─────────────────────
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "ESP-NOW til node: OK" : "ESP-NOW til node: FEIL");
}

// ─── Hjelpefunksjon: fyll inn anker-felt i JSON ───────
static void fillAnchor(JsonObject obj, const AnchorData &a) {
    obj["distance"]     = a.distance;
    obj["raw"]          = a.raw;
    obj["rssi"]         = a.rssi;
    obj["fp_rssi"]      = a.fp_rssi;
    obj["round_time"]   = a.round_time;
    obj["reply_time"]   = a.reply_time;
    obj["clock_offset"] = a.clock_offset;
}

// ─── Mottatt fra node ─────────────────────────────────
void onDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len) {
    int nodeIndex = -1;
    for (int i = 0; i < NODE_COUNT; i++) {
        if (memcmp(mac_addr, nodeMacs[i], 6) == 0) {
            nodeIndex = i;
            lastNodeRecv[i] = millis();
            break;
        }
    }

    // ── Node 1: Isbil ──
    if (nodeIndex == 0) {
        if (len < (int)sizeof(Isbil)) {
            Serial.println("Node 1: data for kort");
            return;
        }
        Isbil isbil;
        memcpy(&isbil, data, sizeof(Isbil));

        Serial.printf("Node 1 (isbil) — balance: %.2f, speed: %.2f, battery: %.2f, iceCreams: %d\n",
            isbil.balance, isbil.speed, isbil.batteryLevel, isbil.iceCreams);

        JsonDocument doc;
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

    // ── Node 2: UWB ──
    else if (nodeIndex == 1) {
        if (len < (int)sizeof(UWBTag)) {
            Serial.println("Node 2: data for kort");
            return;
        }
        UWBTag tag;
        memcpy(&tag, data, sizeof(UWBTag));

        Serial.printf("Node 2 (UWB) — tag_id: %d\n", tag.tag_id);

        JsonDocument doc;
        JsonArray arr = doc.to<JsonArray>();
        JsonObject entry   = arr.add<JsonObject>();
        entry["tag_id"]    = tag.tag_id;
        JsonObject anchors = entry["anchors"].to<JsonObject>();
        fillAnchor(anchors["A1"].to<JsonObject>(), tag.A1);
        fillAnchor(anchors["A2"].to<JsonObject>(), tag.A2);
        fillAnchor(anchors["A3"].to<JsonObject>(), tag.A3);

        char jsonBuf[512];
        serializeJson(doc, jsonBuf);
        Serial.printf("Sender JSON: %s\n", jsonBuf);
        ws.sendTXT(jsonBuf);
    }

    else {
        Serial.printf("Ukjent node — MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
            mac_addr[0], mac_addr[1], mac_addr[2],
            mac_addr[3], mac_addr[4], mac_addr[5]);
    }
}

// ─── Mottatt fra server: JSON → ZumoInstructions → node ──
void onWsEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED:
            Serial.println("WS tilkoblet til server");
            break;

        case WStype_TEXT: {
            Serial.printf("Fra server: %s\n", payload);

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, payload);
            if (err) {
                Serial.println("JSON parse feil");
                break;
            }

            ZumoInstructions instr;
            instr.trafficLightState = doc["trafficLightState"] | false;
            const char *turn = doc["nextTurn"] | "F";
            instr.nextTurn = turn[0];

            Serial.printf("Sender til node — trafficLight: %d, nextTurn: %c\n",
                instr.trafficLightState, instr.nextTurn);

            int receiverId = doc["receiverId"] | 0;

            if (receiverId == 0) {
                for (int i = 0; i < NODE_COUNT; i++) {
                    esp_now_send(nodeMacs[i], (uint8_t*)&instr, sizeof(ZumoInstructions));
                }
            } else if (receiverId >= 1 && receiverId <= NODE_COUNT) {
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
    ws.setReconnectInterval(500);
    ws.enableHeartbeat(15000, 3000, 2);
}

void loop() {
    ws.loop();

    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 3000) {
        lastPrint = millis();
        Serial.printf("[Status] WiFi: %s | WS: %s\n",
            WiFi.status() == WL_CONNECTED ? "OK" : "FEIL",
            ws.isConnected() ? "tilkoblet" : "frakoblet");
        for (int i = 0; i < NODE_COUNT; i++) {
            if (lastNodeRecv[i] == 0) {
                Serial.printf("  Node %d: ingen data mottatt\n", i + 1);
            } else {
                unsigned long siden = (millis() - lastNodeRecv[i]) / 1000;
                Serial.printf("  Node %d: sist hørt for %lu sek siden\n", i + 1, siden);
            }
        }
    }
}
