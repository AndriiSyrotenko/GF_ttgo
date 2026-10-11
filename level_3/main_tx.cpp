#include <Arduino.h>
#include <SPI.h>
#include <RadioLib.h>

// ======================================================
// TTGO LoRa32 T3 v1.6.1 - FSK TRANSMITTER (100 kbps)
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
// TEST CONFIGURATION: 100 kbps / 100 kHz dev / 250 kHz RX BW
// ======================================================

#define TEST_BITRATE 100.0  // кбіт/с
#define TEST_FDEV    100.0  // кГц
#define TEST_RX_BW   250.0  // кГц (максимум для SX1276)

uint8_t payload[64];

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("========================================");
    Serial.println("FSK TRANSMITTER - 100 kbps TEST");
    Serial.println("========================================");

    Serial.print("Bitrate: ");
    Serial.print(TEST_BITRATE * 1000);
    Serial.println(" bps");

    Serial.print("Frequency Deviation: ");
    Serial.print(TEST_FDEV);
    Serial.println(" kHz");

    Serial.println("Packet size: 64 bytes");

    loraSPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

    // Ініціалізація зі збільшеною преамбулою (32 біти замість 16)
    int state = radio.beginFSK(
        915.0,         
        TEST_BITRATE,  
        TEST_FDEV,     
        TEST_RX_BW,    
        17,            
        32,            // Збільшена преамбула для синхронізації на високій швидкості
        false          
    );

    if (state != RADIOLIB_ERR_NONE) {
        Serial.print("FSK initialization FAILED: ");
        Serial.println(state);
        while (true) { delay(1000); }
    }

    // Фіксована довжина пакета 64 байти
    radio.fixedPacketLengthMode(64);

    for (int i = 0; i < 64; i++) {
        payload[i] = 'A' + (i % 26);
    }

    Serial.println();
    Serial.println("Radio initialized successfully in FSK 100k.");

    uint32_t toa = radio.getTimeOnAir(64);
    Serial.print("Theoretical time-on-air: ");
    Serial.print(toa);
    Serial.println(" us");

    Serial.println();
    Serial.println("Starting transmission in 3 seconds...");
    delay(3000);
}

void loop() {
    Serial.println();
    Serial.println("----------------------------------------");
    Serial.println("TRANSMITTING 64 BYTES (FSK 100k)");

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