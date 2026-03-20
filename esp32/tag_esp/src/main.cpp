#include <SPI.h>
#include <esp_now.h>
#include <WiFi.h>

// ─── ESP-NOW konfigurasjon ────────────────────────────────────────────────────
// MAC-adresse til bridge ESP32 – kjør bridge én gang for å finne denne!
// Bridge skriver ut MAC ved oppstart: "Bridge MAC: XX:XX:XX:XX:XX:XX"
uint8_t BRIDGE_MAC[] = {0x00, 0x4B, 0x12, 0x3B, 0x7C, 0x3C};  // <- ENDRE DENNE

// ESP-NOW meldingsstruktur – maks 250 bytes payload
#define ESPNOW_MAX_LEN 250
typedef struct {
    char json[ESPNOW_MAX_LEN];
} EspNowMessage;

EspNowMessage outMsg;
esp_now_peer_info_t peerInfo;
bool espNowReady = false;

// ─── SPI Setup ────────────────────────────────────────────────────────────────
#define RST_PIN 27
#define CHIP_SELECT_PIN 4

// ─── Anchor konfigurasjon ─────────────────────────────────────────────────────
#define NUM_ANCHORS 3
#define TAG_ID 10
#define FIRST_ANCHOR_ID 1

// ─── Ranging konfigurasjon ────────────────────────────────────────────────────
#define FILTER_SIZE 30
#define MIN_DISTANCE 0
#define MAX_DISTANCE 1000.0

// ─── UWB konfigurasjon (uendret) ──────────────────────────────────────────────
#define LEN_RX_CAL_CONF 4
#define LEN_TX_FCTRL_CONF 6
#define LEN_AON_DIG_CFG_CONF 3
#define PMSC_STATE_IDLE 0x3
#define FCS_LEN 2
#define STDRD_SYS_CONFIG 0x188
#define DTUNE0_CONFIG 0x0F
#define SYS_STATUS_FRAME_RX_SUCC 0x2000
#define SYS_STATUS_RX_ERR 0x4279000
#define SYS_STATUS_FRAME_TX_SUCC 0x80
#define PREAMBLE_32 4
#define PREAMBLE_64 8
#define PREAMBLE_128 5
#define PREAMBLE_256 9
#define PREAMBLE_512 11
#define PREAMBLE_1024 2
#define PREAMBLE_2048 10
#define PREAMBLE_4096 3
#define PREAMBLE_1536 6
#define CHANNEL_5 0x0
#define CHANNEL_9 0x1
#define PAC4 0x03
#define PAC8 0x00
#define PAC16 0x01
#define PAC32 0x02
#define DATARATE_6_8MB 0x1
#define DATARATE_850KB 0x0
#define PHR_MODE_STANDARD 0x0
#define PHR_MODE_LONG 0x1
#define PHR_RATE_6_8MB 0x1
#define PHR_RATE_850KB 0x0
#define SPIRDY_MASK 0x80
#define RCINIT_MASK 0x100
#define BIAS_CTRL_BIAS_MASK 0x1F
#define GEN_CFG_AES_LOW_REG 0x00
#define GEN_CFG_AES_HIGH_REG 0x01
#define STS_CFG_REG 0x2
#define RX_TUNE_REG 0x3
#define EXT_SYNC_REG 0x4
#define GPIO_CTRL_REG 0x5
#define DRX_REG 0x6
#define RF_CONF_REG 0x7
#define RF_CAL_REG 0x8
#define FS_CTRL_REG 0x9
#define AON_REG 0xA
#define OTP_IF_REG 0xB
#define CIA_REG1 0xC
#define CIA_REG2 0xD
#define CIA_REG3 0xE
#define DIG_DIAG_REG 0xF
#define PMSC_REG 0x11
#define RX_BUFFER_0_REG 0x12
#define RX_BUFFER_1_REG 0x13
#define TX_BUFFER_REG 0x14
#define ACC_MEM_REG 0x15
#define SCRATCH_RAM_REG 0x16
#define AES_RAM_REG 0x17
#define SET_1_2_REG 0x18
#define INDIRECT_PTR_A_REG 0x1D
#define INDIRECT_PTR_B_REG 0x1E
#define IN_PTR_CFG_REG 0x1F
#define TRANSMIT_DELAY 0x3B9ACA00
#define TRANSMIT_DIFF 0x1FF
#define NS_UNIT 4.0064102564102564
#define PS_UNIT 15.6500400641025641
#define SPEED_OF_LIGHT 0.029979245800
#define CLOCK_OFFSET_CHAN_5_CONSTANT -0.5731e-3f
#define CLOCK_OFFSET_CHAN_9_CONSTANT -0.1252e-3f
#define NO_OFFSET 0x0
#define DEBUG_OUTPUT 0

static int ANTENNA_DELAY = 16350;
int led_status = 0;
int destination = 0x0;
int sender = 0x0;

// Radiokonfigurasjon (uendret)
int config[] = {
    CHANNEL_5,
    PREAMBLE_128,
    9,
    PAC8,
    DATARATE_6_8MB,
    PHR_MODE_STANDARD,
    PHR_RATE_850KB
};

static int rx_status;
static int tx_status;
static int current_anchor_index = 0;
static int curr_stage = 0;

// ─── AnchorData struktur (uendret) ────────────────────────────────────────────
struct AnchorData {
    int anchor_id;
    int t_roundA = 0;
    int t_replyA = 0;
    long long rx = 0;
    long long tx = 0;
    int clock_offset = 0;
    float distance = 0;
    float distance_history[FILTER_SIZE] = {0};
    int history_index = 0;
    float filtered_distance = 0;
    float signal_strength = 0;
    float fp_signal_strength = 0;
};

AnchorData anchors[NUM_ANCHORS];

void initializeAnchors() {
    for (int i = 0; i < NUM_ANCHORS; i++) {
        anchors[i].anchor_id = FIRST_ANCHOR_ID + i;
    }
}

AnchorData *getCurrentAnchor()    { return &anchors[current_anchor_index]; }
int         getCurrentAnchorId()  { return anchors[current_anchor_index].anchor_id; }
void        switchToNextAnchor()  { current_anchor_index = (current_anchor_index + 1) % NUM_ANCHORS; }

bool allAnchorsHaveValidData() {
    for (int i = 0; i < NUM_ANCHORS; i++) {
        if (anchors[i].filtered_distance <= 0) return false;
    }
    return true;
}

// ─── DWM3000 klasse (uendret) ─────────────────────────────────────────────────
class DWM3000Class {
public:
    static int config[9];
    static void spiSelect(uint8_t cs);
    static void begin();
    static void init();
    static void writeSysConfig();
    static void configureAsTX();
    static void setupGPIO();
    static void ds_sendFrame(int stage);
    static void ds_sendRTInfo(int t_roundB, int t_replyB);
    static int  ds_processRTInfo(int t_roundA, int t_replyA, int t_roundB, int t_replyB, int clock_offset);
    static int  ds_getStage();
    static bool ds_isErrorFrame();
    static void ds_sendErrorFrame();
    static void setChannel(uint8_t data);
    static void setPreambleLength(uint8_t data);
    static void setPreambleCode(uint8_t data);
    static void setPACSize(uint8_t data);
    static void setDatarate(uint8_t data);
    static void setPHRMode(uint8_t data);
    static void setPHRRate(uint8_t data);
    static void setMode(int mode);
    static void setTXFrame(unsigned long long frame_data);
    static void setFrameLength(int frame_len);
    static void setTXAntennaDelay(int delay);
    static void setSenderID(int senderID);
    static void setDestinationID(int destID);
    static int  receivedFrameSucc();
    static int  sentFrameSucc();
    static int  getSenderID();
    static int  getDestinationID();
    static bool checkForIDLE();
    static bool checkSPI();
    static double      getSignalStrength();
    static double      getFirstPathSignalStrength();
    static int         getTXAntennaDelay();
    static long double getClockOffset();
    static long double getClockOffset(int32_t ext_clock_offset);
    static int         getRawClockOffset();
    static float       getTempInC();
    static unsigned long long readRXTimestamp();
    static unsigned long long readTXTimestamp();
    static uint32_t write(int base, int sub, uint32_t data, int data_len);
    static uint32_t write(int base, int sub, uint32_t data);
    static uint32_t read(int base, int sub);
    static uint8_t  read8bit(int base, int sub);
    static uint32_t readOTP(uint8_t addr);
    static void writeTXDelay(uint32_t delay);
    static void prepareDelayedTX();
    static void delayedTXThenRX();
    static void delayedTX();
    static void standardTX();
    static void standardRX();
    static void TXInstantRX();
    static void softReset();
    static void hardReset();
    static void clearSystemStatus();
    static void pullLEDHigh(int led);
    static void pullLEDLow(int led);
    static double convertToCM(int DWM3000_ps_units);
    static void calculateTXRXdiff();
    static void printRoundTripInformation();
    static void printDouble(double val, unsigned int precision, bool linebreak);
private:
    static void setBit(int reg_addr, int sub_addr, int shift, bool b);
    static void setBitLow(int reg_addr, int sub_addr, int shift);
    static void setBitHigh(int reg_addr, int sub_addr, int shift);
    static void writeFastCommand(int cmd);
    static uint32_t readOrWriteFullAddress(uint32_t base, uint32_t sub, uint32_t data, uint32_t data_len, uint32_t readWriteBit);
    static uint32_t sendBytes(int b[], int lenB, int recLen);
    static void clearAONConfig();
    static unsigned int countBits(unsigned int number);
    static int checkForDevID();
};

DWM3000Class DWM3000;
int DWM3000Class::config[9];

// ─── ESP-NOW send-callback ────────────────────────────────────────────────────
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status != ESP_NOW_SEND_SUCCESS) {
        Serial.println("[ESP-NOW] Send FAILED");
    } else {
        Serial.println("[ESP-NOW] Send OK");
    }
}

// ─── ESP-NOW initialisering ───────────────────────────────────────────────────
void initEspNow() {
    // WiFi-modul må være aktiv for ESP-NOW, men ingen nettverkstilkobling trengs
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    Serial.print("[TAG] Min MAC: ");
    Serial.println(WiFi.macAddress());

    if (esp_now_init() != ESP_OK) {
        Serial.println("[ERROR] ESP-NOW init mislyktes!");
        return;
    }

    esp_now_register_send_cb(onDataSent);

    memcpy(peerInfo.peer_addr, BRIDGE_MAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("[ERROR] Kunne ikke legge til ESP-NOW peer!");
        return;
    }

    espNowReady = true;
    Serial.println("[ESP-NOW] Klar – sender til bridge");
}

void sendDataViaEspNow() {
    if (!espNowReady) {
        Serial.println("[ESP-NOW] Ikke klar, hopper over sending");
        return;
    }

    String data = "{\"tag_id\":" + String(TAG_ID) + ",\"anchors\":{";
    for (int i = 0; i < NUM_ANCHORS; i++) {
        data += "\"A" + String(anchors[i].anchor_id) + "\":{";
        data += "\"distance\":" + String(anchors[i].filtered_distance, 2) + ",";
        data += "\"rssi\":"     + String(anchors[i].signal_strength, 2);
        data += "}";
        if (i < NUM_ANCHORS - 1) data += ",";
    }
    data += "}}";

    if (data.length() >= ESPNOW_MAX_LEN) {
        Serial.printf("[ERROR] JSON %d bytes – for stor!\n", data.length());
        return;
    }

    data.toCharArray(outMsg.json, ESPNOW_MAX_LEN);
    esp_err_t result = esp_now_send(BRIDGE_MAC, (uint8_t *)&outMsg, sizeof(outMsg));
    if (result != ESP_OK) {
        Serial.println("[ESP-NOW] esp_now_send feilet");
    }
    Serial.printf("[ESP-NOW] Sendt %d bytes\n", data.length());
}

// ─── Hjelpefunksjoner (uendret) ───────────────────────────────────────────────
bool isValidDistance(float distance) {
    return (distance >= MIN_DISTANCE && distance <= MAX_DISTANCE);
}

float calculateMedian(float arr[], int size) {
    float temp[size];
    for (int i = 0; i < size; i++) temp[i] = arr[i];
    for (int i = 0; i < size - 1; i++)
        for (int j = i + 1; j < size; j++)
            if (temp[j] < temp[i]) { float t = temp[i]; temp[i] = temp[j]; temp[j] = t; }
    return (size % 2 == 0) ? (temp[size/2-1] + temp[size/2]) / 2.0 : temp[size/2];
}

void updateFilteredDistance(AnchorData &data) {
    data.distance_history[data.history_index] = data.distance;
    data.history_index = (data.history_index + 1) % FILTER_SIZE;
    float valid_distances[FILTER_SIZE];
    int valid_count = 0;
    for (int i = 0; i < FILTER_SIZE; i++)
        if (isValidDistance(data.distance_history[i]))
            valid_distances[valid_count++] = data.distance_history[i];
    data.filtered_distance = (valid_count > 0) ? calculateMedian(valid_distances, valid_count) : 0;
}

void printAllDistances() {
    Serial.print("Distances - ");
    for (int i = 0; i < NUM_ANCHORS; i++) {
        Serial.print("A"); Serial.print(anchors[i].anchor_id); Serial.print(": ");
        if (anchors[i].filtered_distance > 0) {
            DWM3000.printDouble(anchors[i].filtered_distance, 100, false);
            Serial.print(" cm");
        } else {
            Serial.print("INVALID");
        }
        if (i < NUM_ANCHORS - 1) Serial.print(" | ");
    }
    Serial.println();
}

// ─── DWM3000 implementasjon (identisk med original) ───────────────────────────
void DWM3000Class::spiSelect(uint8_t cs) { pinMode(cs, OUTPUT); digitalWrite(cs, HIGH); delay(5); }

void DWM3000Class::begin() {
    delay(5); pinMode(CHIP_SELECT_PIN, OUTPUT); SPI.begin(); delay(5);
    spiSelect(CHIP_SELECT_PIN); Serial.println("[INFO] SPI ready");
}

void DWM3000Class::init() {
    if (!checkForDevID()) { Serial.println("[ERROR] Dev ID is wrong! Aborting!"); return; }
    setBitHigh(GEN_CFG_AES_LOW_REG, 0x10, 4);
    while (!checkForIDLE()) { Serial.println("[WARNING] IDLE FAILED (stage 1)"); delay(100); }
    softReset(); delay(200);
    while (!checkForIDLE()) { Serial.println("[WARNING] IDLE FAILED (stage 2)"); delay(100); }
    uint32_t ldo_low = readOTP(0x04), ldo_high = readOTP(0x05), bias_tune = readOTP(0xA);
    bias_tune = (bias_tune >> 16) & BIAS_CTRL_BIAS_MASK;
    if (ldo_low != 0 && ldo_high != 0 && bias_tune != 0) { write(0x11, 0x1F, bias_tune); write(0x0B, 0x08, 0x0100); }
    int xtrim_value = readOTP(0x1E);
    xtrim_value = xtrim_value == 0 ? 0x2E : xtrim_value;
    write(FS_CTRL_REG, 0x14, xtrim_value);
    writeSysConfig();
    write(0x00, 0x3C, 0xFFFFFFFF); write(0x00, 0x40, 0xFFFF); write(0x0A, 0x00, 0x000900, 3);
    write(0x3, 0x1C, 0x10000240); write(0x3, 0x20, 0x1B6DA489); write(0x3, 0x38, 0x0001C0FD);
    write(0x3, 0x3C, 0x0001C43E); write(0x3, 0x40, 0x0001C6BE); write(0x3, 0x44, 0x0001C77E);
    write(0x3, 0x48, 0x0001CF36); write(0x3, 0x4C, 0x0001CFB5); write(0x3, 0x50, 0x0001CFF5);
    write(0x3, 0x18, 0xE5E5); int f = read(0x4, 0x20);
    write(0x6, 0x0, 0x81101C); write(0x07, 0x34, 0x4); write(0x07, 0x48, 0x14);
    write(0x07, 0x1A, 0x0E); write(0x07, 0x1C, 0x1C071134); write(0x09, 0x00, 0x1F3C);
    write(0x09, 0x80, 0x81); write(0x11, 0x04, 0xB40200); write(0x11, 0x08, 0x80030738);
    Serial.println("[INFO] Initialization finished.\n");
}

void DWM3000Class::writeSysConfig() {
    int usr_cfg = (STDRD_SYS_CONFIG & 0xFFF) | (config[5] << 3) | (config[6] << 4);
    write(GEN_CFG_AES_LOW_REG, 0x10, usr_cfg);
    int otp_write = 0x1400;
    if (config[1] >= 256) otp_write |= 0x04;
    write(OTP_IF_REG, 0x08, otp_write); write(DRX_REG, 0x00, 0x00, 1); write(DRX_REG, 0x0, config[3]);
    write(STS_CFG_REG, 0x0, 64 / 8 - 1); write(GEN_CFG_AES_LOW_REG, 0x29, 0x00, 1);
    write(DRX_REG, 0x0C, 0xAF5F584C);
    int chan_ctrl_val = read(GEN_CFG_AES_HIGH_REG, 0x14);
    chan_ctrl_val &= (~0x1FFF); chan_ctrl_val |= config[0];
    chan_ctrl_val |= 0x1F00 & (config[2] << 8); chan_ctrl_val |= 0xF8 & (config[2] << 3);
    chan_ctrl_val |= 0x06 & (0x01 << 1); write(GEN_CFG_AES_HIGH_REG, 0x14, chan_ctrl_val);
    int tx_fctrl_val = read(GEN_CFG_AES_LOW_REG, 0x24);
    tx_fctrl_val |= (config[1] << 12); tx_fctrl_val |= (config[4] << 10);
    write(GEN_CFG_AES_LOW_REG, 0x24, tx_fctrl_val); write(DRX_REG, 0x02, 0x81);
    int rf_tx_ctrl_2 = 0x1C071134, pll_conf = 0x0F3C;
    if (config[0]) { rf_tx_ctrl_2 &= ~0x00FFFF; rf_tx_ctrl_2 |= 0x000001; pll_conf &= 0x00FF; pll_conf |= 0x001F; }
    write(RF_CONF_REG, 0x1C, rf_tx_ctrl_2); write(FS_CTRL_REG, 0x00, pll_conf);
    write(RF_CONF_REG, 0x51, 0x14); write(RF_CONF_REG, 0x1A, 0x0E);
    write(FS_CTRL_REG, 0x08, 0x81); write(GEN_CFG_AES_LOW_REG, 0x44, 0x02);
    write(PMSC_REG, 0x04, 0x300200); write(PMSC_REG, 0x08, 0x0138);
    int success = 0;
    for (int i = 0; i < 100; i++) if (read(GEN_CFG_AES_LOW_REG, 0x0) & 0x2) { success = 1; break; }
    Serial.println(success ? "[INFO] PLL is now locked." : "[ERROR] Couldn't lock PLL Clock!");
    int otp_val = read(OTP_IF_REG, 0x08); otp_val |= 0x40;
    if (config[0]) otp_val |= 0x2000; write(OTP_IF_REG, 0x08, otp_val);
    write(RX_TUNE_REG, 0x19, 0xF0);
    int ldo_ctrl_val = read(RF_CONF_REG, 0x48);
    write(RF_CONF_REG, 0x48, (0x105 | 0x100 | 0x4 | 0x1));
    write(EXT_SYNC_REG, 0x0C, 0x020000); int l = read(0x04, 0x0C); delay(20);
    write(EXT_SYNC_REG, 0x0C, 0x11);
    int succ = 0;
    for (int i = 0; i < 100; i++) { if (read(EXT_SYNC_REG, 0x20)) { succ = 1; break; } delay(10); }
    Serial.println(succ ? "[INFO] PGF calibration complete." : "[ERROR] PGF calibration failed!");
    write(EXT_SYNC_REG, 0x0C, 0x00); write(EXT_SYNC_REG, 0x20, 0x01);
    if (read(EXT_SYNC_REG, 0x14) == 0x1fffffff) Serial.println("[ERROR] PGF_CAL failed in stage I!");
    if (read(EXT_SYNC_REG, 0x1C) == 0x1fffffff) Serial.println("[ERROR] PGF_CAL failed in stage Q!");
    write(RF_CONF_REG, 0x48, ldo_ctrl_val); write(0x0E, 0x02, 0x01);
    setTXAntennaDelay(ANTENNA_DELAY);
}

void DWM3000Class::configureAsTX() { write(RF_CONF_REG, 0x1C, 0x34); write(GEN_CFG_AES_HIGH_REG, 0x0C, 0xFDFDFDFD); }
void DWM3000Class::setupGPIO()     { write(0x05, 0x08, 0xF0); }

void DWM3000Class::ds_sendFrame(int stage) {
    setMode(1); write(0x14, 0x01, sender & 0xFF); write(0x14, 0x02, destination & 0xFF);
    write(0x14, 0x03, stage & 0x7); setFrameLength(4); TXInstantRX();
    bool error = true;
    for (int i = 0; i < 50; i++) if (sentFrameSucc()) { error = false; break; }
    if (error) Serial.println("[ERROR] Could not send frame successfully!");
}

void DWM3000Class::ds_sendRTInfo(int t_roundB, int t_replyB) {
    setMode(1); write(0x14, 0x01, destination & 0xFF); write(0x14, 0x02, sender & 0xFF);
    write(0x14, 0x03, 4); write(0x14, 0x04, t_roundB); write(0x14, 0x08, t_replyB);
    setFrameLength(12); TXInstantRX();
}

int DWM3000Class::ds_processRTInfo(int t_roundA, int t_replyA, int t_roundB, int t_replyB, int clk_offset) {
    int reply_diff = t_replyA - t_replyB;
    long double clock_offset = t_replyA > t_replyB ? 1.0 + getClockOffset(clk_offset) : 1.0 - getClockOffset(clk_offset);
    int first_rt = t_roundA - t_replyB, second_rt = t_roundB - t_replyA;
    int combined_rt = (first_rt + second_rt - (reply_diff - (reply_diff * clock_offset))) / 2;
    return combined_rt / 2;
}

int  DWM3000Class::ds_getStage()    { return read(0x12, 0x03) & 0b111; }
bool DWM3000Class::ds_isErrorFrame() { return ((read(0x12, 0x00) & 0x7) == 7); }
void DWM3000Class::ds_sendErrorFrame() { Serial.println("[WARNING] Error Frame sent."); setMode(7); setFrameLength(3); standardTX(); }

void DWM3000Class::setChannel(uint8_t d)       { if (d==CHANNEL_5||d==CHANNEL_9) config[0]=d; }
void DWM3000Class::setPreambleLength(uint8_t d) { config[1]=d; }
void DWM3000Class::setPreambleCode(uint8_t d)  { if (d<=12&&d>=9) config[2]=d; }
void DWM3000Class::setPACSize(uint8_t d)       { config[3]=d; }
void DWM3000Class::setDatarate(uint8_t d)      { if (d==DATARATE_6_8MB||d==DATARATE_850KB) config[4]=d; }
void DWM3000Class::setPHRMode(uint8_t d)       { if (d==PHR_MODE_STANDARD||d==PHR_MODE_LONG) config[5]=d; }
void DWM3000Class::setPHRRate(uint8_t d)       { if (d==PHR_RATE_6_8MB||d==PHR_RATE_850KB) config[6]=d; }
void DWM3000Class::setMode(int mode)           { write(0x14, 0x00, mode & 0x7); }
void DWM3000Class::setTXFrame(unsigned long long fd) { write(TX_BUFFER_REG, 0x00, fd); }

void DWM3000Class::setFrameLength(int frameLen) {
    frameLen += FCS_LEN; int curr_cfg = read(0x00, 0x24);
    if (frameLen > 1023) { Serial.println("[ERROR] Frame too long!"); return; }
    write(GEN_CFG_AES_LOW_REG, 0x24, (curr_cfg & 0xFFFFFC00) | frameLen);
}

void DWM3000Class::setTXAntennaDelay(int delay) { ANTENNA_DELAY = delay; write(0x01, 0x04, delay); }
void DWM3000Class::setSenderID(int id)          { sender = id; }
void DWM3000Class::setDestinationID(int id)     { destination = id; }

int DWM3000Class::receivedFrameSucc() {
    int s = read(GEN_CFG_AES_LOW_REG, 0x44);
    if (s & SYS_STATUS_FRAME_RX_SUCC) return 1;
    if (s & SYS_STATUS_RX_ERR)        return 2;
    return 0;
}
int  DWM3000Class::sentFrameSucc()   { return (read(GEN_CFG_AES_LOW_REG,0x44)&SYS_STATUS_FRAME_TX_SUCC)==SYS_STATUS_FRAME_TX_SUCC?1:0; }
int  DWM3000Class::getSenderID()     { return read(0x12,0x01)&0xFF; }
int  DWM3000Class::getDestinationID(){ return read(0x12,0x02)&0xFF; }
bool DWM3000Class::checkForIDLE()    { return (read(0x0F,0x30)>>16&PMSC_STATE_IDLE)==PMSC_STATE_IDLE||(read(0x00,0x44)>>16&(SPIRDY_MASK|RCINIT_MASK))==(SPIRDY_MASK|RCINIT_MASK)?1:0; }
bool DWM3000Class::checkSPI()        { return checkForDevID(); }

double DWM3000Class::getSignalStrength() {
    int C=read(0x0C,0x2C)&0x1FF, P=read(0x0C,0x58)&0xFFF;
    unsigned int D=(read(0x03,0x60)>>28)&0x7;
    return 10*log10((C*(1<<21))/pow(P,2))+(6*D)-121.7;
}
double DWM3000Class::getFirstPathSignalStrength() {
    float f1=(read(0x0C,0x30)&0x3FFFFF)>>2, f2=(read(0x0C,0x34)&0x3FFFFF)>>2, f3=(read(0x0C,0x38)&0x3FFFFF)>>2;
    int P=read(0x0C,0x58)&0xFFF; unsigned int D=(read(0x03,0x60)>>28)&0x7;
    return 10*log10((pow(f1,2)+pow(f2,2)+pow(f3,2))/pow(P,2))+(6*D)-121.7;
}
int DWM3000Class::getTXAntennaDelay() { return read(0x01,0x04)&0xFFFF; }

long double DWM3000Class::getClockOffset() {
    return getRawClockOffset()*(config[0]==CHANNEL_5?CLOCK_OFFSET_CHAN_5_CONSTANT:CLOCK_OFFSET_CHAN_9_CONSTANT)/1000000;
}
long double DWM3000Class::getClockOffset(int32_t o) {
    return o*(config[0]==CHANNEL_5?CLOCK_OFFSET_CHAN_5_CONSTANT:CLOCK_OFFSET_CHAN_9_CONSTANT)/1000000;
}
int DWM3000Class::getRawClockOffset() {
    int r=read(0x06,0x29)&0x1FFFFF;
    if (r&(1<<20)) r|=~((1<<21)-1);
    return r;
}
float DWM3000Class::getTempInC() {
    write(0x07,0x34,0x04); write(0x08,0x00,0x01);
    while(!(read(0x08,0x04)&0x01));
    int res=(read(0x08,0x08)&0xFF00)>>8, otp=readOTP(0x09)&0xFF;
    float t=(float)((res-otp)*1.05f)+22.0f; write(0x08,0x00,0x00,1); return t;
}
unsigned long long DWM3000Class::readRXTimestamp() {
    uint32_t l=read(0x0C,0x00); unsigned long long h=read(0x0C,0x04)&0xFF;
    return (h<<32)|l;
}
unsigned long long DWM3000Class::readTXTimestamp() {
    unsigned long long l=read(0x00,0x74), h=read(0x00,0x78)&0xFF;
    return (h<<32)+l;
}
uint32_t DWM3000Class::write(int b,int s,uint32_t d,int l) { return readOrWriteFullAddress(b,s,d,l,1); }
uint32_t DWM3000Class::write(int b,int s,uint32_t d)       { return readOrWriteFullAddress(b,s,d,0,1); }
uint32_t DWM3000Class::read(int b,int s)  { return readOrWriteFullAddress(b,s,0,0,0); }
uint8_t  DWM3000Class::read8bit(int b,int s) { return (uint8_t)(read(b,s)>>24); }
uint32_t DWM3000Class::readOTP(uint8_t addr) { write(OTP_IF_REG,0x04,addr); write(OTP_IF_REG,0x08,0x02); return read(OTP_IF_REG,0x10); }
void DWM3000Class::writeTXDelay(uint32_t d) { write(0x00,0x2C,d); }
void DWM3000Class::prepareDelayedTX() {
    long long rx_ts=readRXTimestamp();
    uint32_t etx=(long long)(rx_ts+TRANSMIT_DELAY)>>8;
    long long ctx=((rx_ts+TRANSMIT_DELAY)&~TRANSMIT_DIFF)+ANTENNA_DELAY;
    write(0x14,0x01,sender&0xFF); write(0x14,0x02,destination&0xFF);
    write(0x14,0x03,ctx-rx_ts); setFrameLength(7); writeTXDelay(etx);
}
void DWM3000Class::delayedTXThenRX() { writeFastCommand(0x0F); }
void DWM3000Class::delayedTX()       { writeFastCommand(0x3); }
void DWM3000Class::standardTX()      { writeFastCommand(0x01); }
void DWM3000Class::standardRX()      { writeFastCommand(0x02); }
void DWM3000Class::TXInstantRX()     { writeFastCommand(0x0C); }
void DWM3000Class::softReset() {
    clearAONConfig(); write(PMSC_REG,0x04,0x1); write(PMSC_REG,0x00,0x00,2);
    delay(100); write(PMSC_REG,0x00,0xFFFF); write(PMSC_REG,0x04,0x00,1);
}
void DWM3000Class::hardReset() { pinMode(RST_PIN,OUTPUT); digitalWrite(RST_PIN,LOW); delay(10); pinMode(RST_PIN,INPUT); }
void DWM3000Class::clearSystemStatus() { write(GEN_CFG_AES_LOW_REG,0x44,0x3F7FFFFF); }
void DWM3000Class::pullLEDHigh(int l) { if(l>2)return; led_status|=(1<<l); write(0x05,0x0C,led_status); }
void DWM3000Class::pullLEDLow(int l)  { if(l>2)return; led_status&=~((int)1<<l); write(0x05,0x0C,led_status); }
double DWM3000Class::convertToCM(int u) { return (double)u*PS_UNIT*SPEED_OF_LIGHT; }
void DWM3000Class::calculateTXRXdiff() {
    unsigned long long pt=readTXTimestamp(), pr=readRXTimestamp();
    long double co=1.0+getClockOffset(); long long tr=read(RX_BUFFER_0_REG,0x03);
    if(!tr)return;
    long long ro=pr-pt, tp=lround((ro-lround(tr*co))/2);
    long double tc=tp*PS_UNIT*SPEED_OF_LIGHT;
    if(tc>=0){printDouble(tc,100,false);Serial.println("cm");}
}
void DWM3000Class::printRoundTripInformation() {
    Serial.println("\nRound Trip Information:");
    Serial.print("TX Timestamp: "); Serial.println((long long)readTXTimestamp());
    Serial.print("RX Timestamp: "); Serial.println((long long)readRXTimestamp());
}
void DWM3000Class::printDouble(double val, unsigned int precision, bool linebreak) {
    Serial.print(int(val)); Serial.print(".");
    unsigned int frac = val>=0 ? (val-int(val))*precision : (int(val)-val)*precision;
    if(linebreak) Serial.println(frac,DEC); else Serial.print(frac,DEC);
}
void DWM3000Class::setBit(int r,int s,int sh,bool b) {
    uint8_t t=read8bit(r,s); b?bitSet(t,sh):bitClear(t,sh); write(r,s,t);
}
void DWM3000Class::setBitLow(int r,int s,int sh)  { setBit(r,s,sh,0); }
void DWM3000Class::setBitHigh(int r,int s,int sh) { setBit(r,s,sh,1); }
void DWM3000Class::writeFastCommand(int cmd) {
    int h=0x81|((cmd&0x1F)<<1); int a[]={h}; sendBytes(a,1,0);
}
uint32_t DWM3000Class::readOrWriteFullAddress(uint32_t base,uint32_t sub,uint32_t data,uint32_t dLen,uint32_t rw) {
    uint32_t hdr=0; if(rw) hdr|=0x80; hdr|=((base&0x1F)<<1);
    if(sub>0){hdr|=0x40;hdr<<=8;hdr|=((sub&0x7F)<<2);}
    uint32_t hs=hdr>0xFF?2:1, res=0;
    if(!rw){
        int ha[hs]; if(hs==1)ha[0]=hdr; else{ha[0]=(hdr&0xFF00)>>8;ha[1]=hdr&0xFF;}
        return (uint32_t)sendBytes(ha,hs,4);
    }
    uint32_t pb=0;
    if(dLen==0){if(data>0){uint32_t bits=countBits(data);pb=(bits-(bits%8))/8;if(bits%8)pb++;}else pb=1;}else pb=dLen;
    int pl[hs+pb];
    if(hs==1)pl[0]=hdr; else{pl[0]=(hdr&0xFF00)>>8;pl[1]=hdr&0xFF;}
    for(int i=0;i<(int)pb;i++)pl[hs+i]=(data>>(i*8))&0xFF;
    return (uint32_t)sendBytes(pl,2+pb,0);
}
uint32_t DWM3000Class::sendBytes(int b[],int lb,int rl) {
    digitalWrite(CHIP_SELECT_PIN,LOW);
    for(int i=0;i<lb;i++)SPI.transfer(b[i]);
    uint32_t val=0;
    if(rl>0)for(int i=0;i<rl;i++){uint32_t t=SPI.transfer(0x00);val=i==0?t:val|(t<<8*i);}
    digitalWrite(CHIP_SELECT_PIN,HIGH); return val;
}
void DWM3000Class::clearAONConfig() {
    write(AON_REG,NO_OFFSET,0x00,2); write(AON_REG,0x14,0x00,1);
    write(AON_REG,0x04,0x00,1); write(AON_REG,0x04,0x02); delay(1);
}
unsigned int DWM3000Class::countBits(unsigned int n) { return (int)log2(n)+1; }
int DWM3000Class::checkForDevID() {
    int r=read(GEN_CFG_AES_LOW_REG,NO_OFFSET);
    if(r!=0xDECA0302&&r!=0xDECA0312){Serial.println("[ERROR] DEV_ID IS WRONG!");return 0;}
    return 1;
}

// ─── Setup ────────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    initializeAnchors();

    Serial.print("Initialized "); Serial.print(NUM_ANCHORS); Serial.println(" anchors:");
    for (int i = 0; i < NUM_ANCHORS; i++) {
        Serial.print("  Anchor "); Serial.print(i); Serial.print(" - ID: "); Serial.println(anchors[i].anchor_id);
    }

    // Initialiser ESP-NOW (erstatter WiFi TCP)
    initEspNow();

    // Initialiser UWB
    DWM3000.begin();
    DWM3000.hardReset();
    delay(200);

    if (!DWM3000.checkSPI()) {
        Serial.println("[ERROR] Could not establish SPI Connection to DWM3000!");
        while (1);
    }
    while (!DWM3000.checkForIDLE()) { Serial.println("[ERROR] IDLE1 FAILED\r"); delay(1000); }
    DWM3000.softReset(); delay(200);
    if (!DWM3000.checkForIDLE()) { Serial.println("[ERROR] IDLE2 FAILED\r"); while (1); }

    DWM3000.init();
    DWM3000.setupGPIO();
    DWM3000.setTXAntennaDelay(16350);
    DWM3000.setSenderID(TAG_ID);

    Serial.println("> TAG - ESP-NOW Bridge System <");
    Serial.println("[INFO] Setup is finished.");
    Serial.print("Antenna delay set to: ");
    Serial.println(DWM3000.getTXAntennaDelay());

    DWM3000.configureAsTX();
    DWM3000.clearSystemStatus();
}

// ─── Loop (identisk med original, sendDataOverWiFi → sendDataViaEspNow) ───────
void loop() {
    AnchorData *currentAnchor = getCurrentAnchor();
    int currentAnchorId = getCurrentAnchorId();

    switch (curr_stage) {
    case 0:
        currentAnchor->t_roundA = 0; currentAnchor->t_replyA = 0;
        DWM3000.setDestinationID(currentAnchorId);
        DWM3000.ds_sendFrame(1);
        currentAnchor->tx = DWM3000.readTXTimestamp();
        curr_stage = 1;
        break;

    case 1:
        if (rx_status = DWM3000.receivedFrameSucc()) {
            DWM3000.clearSystemStatus();
            if (rx_status == 1) {
                if (DWM3000.ds_isErrorFrame()) {
                    Serial.print("[WARNING] Error frame from Anchor "); Serial.print(currentAnchorId);
                    Serial.print("! Signal strength: "); Serial.print(DWM3000.getSignalStrength()); Serial.println(" dBm");
                    curr_stage = 0;
                } else if (DWM3000.ds_getStage() != 2) {
                    Serial.print("[WARNING] Unexpected stage from Anchor "); Serial.print(currentAnchorId);
                    Serial.print(": "); Serial.println(DWM3000.ds_getStage());
                    DWM3000.ds_sendErrorFrame(); curr_stage = 0;
                } else {
                    curr_stage = 2;
                }
            } else {
                Serial.print("[ERROR] Receiver Error from Anchor "); Serial.println(currentAnchorId);
                DWM3000.clearSystemStatus();
            }
        }
        break;

    case 2:
        currentAnchor->rx = DWM3000.readRXTimestamp();
        DWM3000.ds_sendFrame(3);
        currentAnchor->t_roundA = currentAnchor->rx - currentAnchor->tx;
        currentAnchor->tx = DWM3000.readTXTimestamp();
        currentAnchor->t_replyA = currentAnchor->tx - currentAnchor->rx;
        curr_stage = 3;
        break;

    case 3:
        if (rx_status = DWM3000.receivedFrameSucc()) {
            DWM3000.clearSystemStatus();
            if (rx_status == 1) {
                if (DWM3000.ds_isErrorFrame()) {
                    Serial.print("[WARNING] Error frame from Anchor "); Serial.println(currentAnchorId);
                    curr_stage = 0;
                } else {
                    currentAnchor->clock_offset = DWM3000.getRawClockOffset();
                    curr_stage = 4;
                }
            } else {
                Serial.print("[ERROR] Receiver Error from Anchor "); Serial.println(currentAnchorId);
                DWM3000.clearSystemStatus();
            }
        }
        break;

    case 4: {
        int ranging_time = DWM3000.ds_processRTInfo(
            currentAnchor->t_roundA, currentAnchor->t_replyA,
            DWM3000.read(0x12, 0x04), DWM3000.read(0x12, 0x08),
            currentAnchor->clock_offset);

        currentAnchor->distance          = DWM3000.convertToCM(ranging_time);
        currentAnchor->signal_strength   = DWM3000.getSignalStrength();
        currentAnchor->fp_signal_strength= DWM3000.getFirstPathSignalStrength();
        updateFilteredDistance(*currentAnchor);

        printAllDistances();

        // ← Eneste endring: sendDataOverWiFi() er byttet ut med sendDataViaEspNow()
        if (allAnchorsHaveValidData()) {
            sendDataViaEspNow();
        }

        switchToNextAnchor();
        curr_stage = 0;
        break;
    }

    default:
        Serial.print("Entered stage ("); Serial.print(curr_stage); Serial.println("). Reverting back to stage 0");
        curr_stage = 0;
        break;
    }
}