#include <Arduino.h>
#include <SPI.h>
#include <RadioLib.h>

// TTGO LoRa32 T3 v1.6.1
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

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("================================");
    Serial.println("TTGO LoRa - RECEIVER");
    Serial.println("================================");

    loraSPI.begin(
        LORA_SCK,
        LORA_MISO,
        LORA_MOSI,
        LORA_CS
    );

    int state = radio.begin(
        915.0,   // Frequency
        125.0,   // Bandwidth
        7,       // Spreading factor
        5,       // Coding rate
        0x12,    // Sync word
        17,      // Output power
        8,       // Preamble
        0        // Gain
    );

    if (state != RADIOLIB_ERR_NONE) {
        Serial.print("LoRa initialization failed, code: ");
        Serial.println(state);

        while (true) {
            delay(1000);
        }
    }

    Serial.println("LoRa initialization: SUCCESS");
    Serial.println("Waiting for packets...");
}

void loop() {

    String message;

    int state = radio.receive(message);

    if (state == RADIOLIB_ERR_NONE) {

        Serial.println();
        Serial.println("========== PACKET RECEIVED ==========");

        Serial.print("Message: ");
        Serial.println(message);

        Serial.print("RSSI: ");
        Serial.print(radio.getRSSI());
        Serial.println(" dBm");

        Serial.print("SNR: ");
        Serial.print(radio.getSNR());
        Serial.println(" dB");

        Serial.println("=====================================");
    }
    else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
        Serial.println("RX timeout");
    }
    else {
        Serial.print("Receive failed, code: ");
        Serial.println(state);
    }
}