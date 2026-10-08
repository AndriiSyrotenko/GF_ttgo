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
    Serial.println("TTGO LoRa - TRANSMITTER");
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
    Serial.println("Starting transmission...");
}

void loop() {

    const char* message = "Hello from TTGO #1";

    Serial.print("Sending: ");
    Serial.println(message);

    int state = radio.transmit(message);

    if (state == RADIOLIB_ERR_NONE) {
        Serial.println("Transmission successful!");
    } else {
        Serial.print("Transmission failed, code: ");
        Serial.println(state);
    }

    delay(1000);
}