#include <Arduino.h>
#include <SPI.h>
#include <RadioLib.h>

// Pin Definitions for LoRa module using RadioLib
#define LoRa_MOSI 10
#define LoRa_MISO 11
#define LoRa_SCK 9
#define LoRa_nss 8
#define LoRa_dio1 14
#define LoRa_nrst 12
#define LoRa_busy 13

SX1262 radio = new Module(LoRa_nss, LoRa_dio1, LoRa_nrst, LoRa_busy);

void setup() {
  Serial.begin(115200);
  Serial.println("Booting LoRa Receiver");

  // Initialize SPI communication for LoRa module
  SPI.begin(LoRa_SCK, LoRa_MISO, LoRa_MOSI, LoRa_nss);

  // Initialize LoRa module
  int state = radio.begin();
  if (state == -1) {
    Serial.println("LoRa module initialization failed!");
    while (1);
  }
  Serial.println("LoRa module initialized successfully");
  radio.setFrequency(915000000);
}

void loop() {
  // Check if there is any packet received
  Serial.println("Waiting...");
  if (radio.available()) {
    // Read the received data
    Serial.println("Checking...");
    uint8_t receivedData;
    int dataSize = radio.receive(&receivedData, sizeof(receivedData));

    // Check if data was received successfully
    if (dataSize > 0) {
      // Print the received data
      Serial.print("Received data: ");
      Serial.println(receivedData);

      // Example of handling the received data
      if (receivedData == 1) {
        // Handle received binary 1
        Serial.println("Received binary 1");
      } else if (receivedData == 0) {
        // Handle received binary 0
        Serial.println("Received binary 0");
      } else {
        // Handle other received values if needed
        Serial.println("Received unexpected value");
      }
    } else {
      Serial.println("Error receiving LoRa packet");
    }
  }

  delay(100);  // Short delay between checking for packets
}
