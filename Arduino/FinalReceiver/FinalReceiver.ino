#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <RadioLib.h>
#include <U8g2lib.h>


// Define LoRa pins
#define LoRa_MOSI 10
#define LoRa_MISO 11
#define LoRa_SCK 9
#define LoRa_nss 8
#define LoRa_dio1 14
#define LoRa_nrst 12
#define LoRa_busy 13

SX1262 radio = new Module(LoRa_nss, LoRa_dio1, LoRa_nrst, LoRa_busy);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/21, /* clock=*/18, /* data=*/17);

// Function to display text on the OLED screen
void displayText(const char* text) {
  u8g2.clearBuffer();               // Clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr); // Choose a suitable font
  u8g2.drawStr(0, 10, text);        // Write something to the internal memory
  u8g2.sendBuffer();                // Transfer internal memory to the display
}


void setup() {
    Serial.begin(115200);
    Serial.println(F("Serial communication initialized."));

    SPI.begin(LoRa_SCK, LoRa_MISO, LoRa_MOSI, LoRa_nss);
    Serial.println(F("SPI communication initialized."));

    // Initialize LoRa module
    Serial.print(F("[SX1262] Initializing ... "));
    int state = radio.begin();
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("Initialization successful!"));
    } else {
        Serial.print(F("Initialization failed, code "));
        Serial.println(state);
        while (true);
    }

    // Set the frequency (in Hz)
    radio.setFrequency(915000000);  // Set to 915 MHz
    radio.setSpreadingFactor(12);
    radio.setBandwidth(125000);
    u8g2.begin();
}

void loop() {
    // Receive a packet
    uint8_t receivedData;
    int state = radio.receive(&receivedData, sizeof(receivedData));

    if (state == RADIOLIB_ERR_NONE) {
        // Packet received successfully
        Serial.println(F("Received packet successfully!"));

        // Print the received data
        Serial.print(F("[SX1262] Received data:\t"));
        Serial.println(receivedData);

        // Example of handling the received data
        if (receivedData == 1) {
            Serial.println(F("Received binary 1"));
            displayText("Power On");
            // Handle binary 1 condition
        } else if (receivedData == 0) {
            Serial.println(F("Received binary 0"));
            displayText("Power Off");
            // Handle binary 0 condition
        } else {
            Serial.println(F("Received unexpected value"));
            // Handle unexpected value condition
        }
    } else if (state != RADIOLIB_ERR_RX_TIMEOUT && state != RADIOLIB_ERR_CRC_MISMATCH) {
        // Some other error occurred (excluding timeout and CRC mismatch)
        Serial.print(F("Failed to receive packet, code "));
        Serial.println(state);
    }
    Serial.println("Waiting...");
    delay(500);  // Short delay between checking for packets
}
