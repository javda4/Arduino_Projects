#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <U8g2lib.h>

// Define the pins based on your board's specification
#define SS 8
#define RST 12 
#define DI0 14

// Initialize the OLED display using U8g2 library
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
  Serial.println("LoRa Receiver");
  u8g2.begin();
  displayText("LoRa Receiver Ready");
  // Initialize LoRa with the specified pins
  LoRa.setPins(SS, RST, DI0);
  
  if (!LoRa.begin(915E6)) { // Use the correct frequency for your region
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  // Initialize the OLED display
  u8g2.begin();
  displayText("LoRa Receiver Ready");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String receivedText = "";
    while (LoRa.available()) {
      receivedText += (char)LoRa.read();
    }

    Serial.println("Received: " + receivedText);
    displayText(receivedText.c_str());
  }
}
