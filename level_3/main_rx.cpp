#include <Arduino.h>
#include <SPI.h>
#include <RadioLib.h>

// ======================================================
// TTGO LoRa32 T3 v1.6.1 - FSK RECEIVER (100 kbps)
// ======================================================

#define LORA_SCK   5
#define LORA_MISO  19
#define LORA_MOSI  27
#define LORA_CS    18
#define LORA_RST   23
#define LORA_DIO0  26
#define LORA_DIO1  33

SPIClass loraSPI(VSPI);

SX1276 radio = new Module(
    LORA_CS,
    LORA_DIO0,
    LORA_RST,
    LORA_DIO1,
    loraSPI
);

// ======================================================
// TEST CONFIGURATION (Має суворо збігатися з TX!)
// ======================================================

#define TEST_BITRATE 100.0  // кбіт/с
#define TEST_FDEV    100.0  // кГц
#define TEST_RX_BW   250.0  // кГц

uint8_t receivedData[64];

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("========================================");
    Serial.println("FSK RECEIVER - 100 kbps TEST");
    Serial.println("========================================");

    Serial.print("Bitrate: ");
    Serial.print(TEST_BITRATE * 1000);
    Serial.println(" bps");

    Serial.print("Frequency Deviation: ");
    Serial.print(TEST_FDEV);
    Serial.println(" kHz");

    Serial.println("Expected packet size: 64 bytes");

    loraSPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

    int state = radio.beginFSK(
        915.0,
        TEST_BITRATE,
        TEST_FDEV,
        TEST_RX_BW,
        17,
        32,            // Преамбула 32 біти для синхронізації
        false
    );

    if (state != RADIOLIB_ERR_NONE) {
        Serial.print("FSK initialization FAILED: ");
        Serial.println(state);
        while (true) { delay(1000); }
    }

    // Синхронізуємо фіксовану довжину пакета
    radio.fixedPacketLengthMode(64);

    Serial.println();
    Serial.println("Radio initialized successfully in FSK 100k.");
    Serial.println("Waiting for 64-byte packets...");
}

void loop() {
    int state = radio.receive(receivedData, 64);

    if (state == RADIOLIB_ERR_NONE) {
        Serial.println();
        Serial.println("========================================");
        Serial.println("PACKET RECEIVED (FSK 100k)");

        Serial.print("Length: ");
        Serial.println(radio.getPacketLength());

        Serial.print("RSSI: ");
        Serial.print(radio.getRSSI());
        Serial.println(" dBm");

        Serial.print("Data: ");
        for (int i = 0; i < 64; i++) {
            Serial.write(receivedData[i]);
        }
        Serial.println();
        Serial.println("========================================");
    }
    else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
        // Час іде, а пакетів усе немає — класика жанру.
    }
    else {
        Serial.print("Receive FAILED: ");
        Serial.println(state);
    }
}