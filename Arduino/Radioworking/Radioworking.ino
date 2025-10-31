#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <RadioLib.h>

// Pin Definitions for LoRa module
#define LoRa_MOSI 10
#define LoRa_MISO 11
#define LoRa_SCK 9

#define LoRa_nss 8
#define LoRa_dio1 14
#define LoRa_nrst 12
#define LoRa_busy 13

// Create an instance of the LoRa module
SX1262 radio = new Module(LoRa_nss, LoRa_dio1, LoRa_nrst, LoRa_busy);

void setup() {
// Initialize Serial communication
Serial.begin(9600);

// Initialize SPI communication for LoRa module
SPI.begin(LoRa_SCK, LoRa_MISO, LoRa_MOSI, LoRa_nss);

// Initialize LoRa module
Serial.print(F("[SX1262] Initializing ... "));
int state = radio.begin();
if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
} else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
}

// Set the frequency (in Hz)
// Range: 150-960 MhZ 900 MHz frequency
radio.setFrequency(915000000);  // 900 MhZ

// Set other configurations as needed
}

void loop() {
// Transmit a message
Serial.print(F("[SX1262] Transmitting packet … "));

// Send the message
int state = radio.transmit("Hello from Transmitter!");

// Check transmission status
if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
} else {
    Serial.print(F("failed, code "));
    Serial.println(state);
}

delay(5000); // Wait for 5 seconds before transmitting again
}