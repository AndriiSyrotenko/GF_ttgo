#include <Arduino.h>
#include <SPI.h>
#include <RadioLib.h>

// ======================================================
// TTGO LoRa32 T3 v1.6.1
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
// TEST CONFIGURATION
// Повинна збігатися з TX
// ======================================================

#define TEST_SF 6
#define TEST_BW 500.0

uint8_t receivedData[64];

void setup() {

    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("========================================");
    Serial.println("LoRa RECEIVER - SPEED TEST");
    Serial.println("========================================");

    Serial.print("SF: ");
    Serial.println(TEST_SF);

    Serial.print("BW: ");
    Serial.print(TEST_BW);
    Serial.println(" kHz");

    Serial.println("Expected packet size: 64 bytes");

    // --------------------------------------------------
    // SPI
    // --------------------------------------------------

    loraSPI.begin(
        LORA_SCK,
        LORA_MISO,
        LORA_MOSI,
        LORA_CS
    );

    // --------------------------------------------------
    // LoRa initialization
    // --------------------------------------------------

    int state = radio.begin(
        915.0,
        TEST_BW,
        TEST_SF,
        5,
        0x12,
        17,
        8,
        0
    );

    if (state != RADIOLIB_ERR_NONE) {

        Serial.print("LoRa initialization FAILED: ");
        Serial.println(state);

        while (true) {
            delay(1000);
        }
    }

    // SF6 requires implicit header.
    if (TEST_SF == 6) {

        state = radio.implicitHeader(64);

        if (state != RADIOLIB_ERR_NONE) {

            Serial.print("Implicit header FAILED: ");
            Serial.println(state);

            while (true) {
                delay(1000);
            }
        }
    }

    Serial.println();
    Serial.println("Radio initialized successfully.");
    Serial.println("Waiting for 64-byte packets...");
}

void loop() {

    int state = radio.receive(receivedData, 64);

    if (state == RADIOLIB_ERR_NONE) {

        Serial.println();
        Serial.println("========================================");
        Serial.println("PACKET RECEIVED");

        Serial.print("Length: ");
        Serial.println(radio.getPacketLength());

        Serial.print("RSSI: ");
        Serial.print(radio.getRSSI());
        Serial.println(" dBm");

        Serial.print("SNR: ");
        Serial.print(radio.getSNR());
        Serial.println(" dB");

        Serial.print("Data: ");

        for (int i = 0; i < 64; i++) {
            Serial.write(receivedData[i]);
        }

        Serial.println();
        Serial.println("========================================");
    }

    else if (state == RADIOLIB_ERR_RX_TIMEOUT) {

        // Nothing to do.
        // Timeout is normal while waiting for the next packet.
    }

    else {

        Serial.print("Receive FAILED: ");
        Serial.println(state);
    }
}