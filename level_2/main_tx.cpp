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
// Змінюємо тільки ці два параметри
// ======================================================

#define TEST_SF 12
#define TEST_BW 500.0

// ======================================================
// 64-byte payload
// ======================================================

uint8_t payload[64];

void setup() {

    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("========================================");
    Serial.println("LoRa TRANSMITTER - SPEED TEST");
    Serial.println("========================================");

    Serial.print("SF: ");
    Serial.println(TEST_SF);

    Serial.print("BW: ");
    Serial.print(TEST_BW);
    Serial.println(" kHz");

    Serial.println("Packet size: 64 bytes");

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
        915.0,       // frequency MHz
        TEST_BW,     // bandwidth kHz
        TEST_SF,     // spreading factor
        5,           // coding rate 4/5
        0x12,        // sync word
        17,          // power dBm
        8,            // preamble
        0             // automatic gain
    );

    if (state != RADIOLIB_ERR_NONE) {

        Serial.print("LoRa initialization FAILED: ");
        Serial.println(state);

        while (true) {
            delay(1000);
        }
    }

    // SF6 requires implicit header mode.
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

    // --------------------------------------------------
    // Create exactly 64 bytes
    // --------------------------------------------------

    for (int i = 0; i < 64; i++) {
        payload[i] = 'A' + (i % 26);
    }

    Serial.println();
    Serial.println("Radio initialized successfully.");

    Serial.print("Theoretical time-on-air: ");

    uint32_t toa = radio.getTimeOnAir(64);

    Serial.print(toa);
    Serial.println(" us");

    Serial.println();
    Serial.println("Starting transmission in 3 seconds...");

    delay(3000);
}

void loop() {

    Serial.println();
    Serial.println("----------------------------------------");
    Serial.println("TRANSMITTING 64 BYTES");

    uint32_t startTime = micros();

    int state = radio.transmit(payload, 64);

    uint32_t endTime = micros();

    uint32_t elapsed = endTime - startTime;

    if (state == RADIOLIB_ERR_NONE) {

        Serial.println("Transmission successful!");

        Serial.print("Measured transmission time: ");
        Serial.print(elapsed);
        Serial.println(" us");

        Serial.print("Measured transmission time: ");
        Serial.print(elapsed / 1000.0, 3);
        Serial.println(" ms");

    } else {

        Serial.print("Transmission FAILED: ");
        Serial.println(state);
    }

    delay(3000);
}