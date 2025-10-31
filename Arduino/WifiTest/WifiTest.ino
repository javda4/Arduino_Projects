#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <U8g2lib.h>

const char* ssid = "Woods";
const char* password = "Woods7811";
const char* server = "api.adalo.com";
const int port = 443;
const char* path = "/v0/apps/614a75c2-7d33-4d68-a979-8e42076d446f/collections/t_d31pae6qu475xevmok3fophxd/48";
const char* bearer_token = "Bearer 0bj9lvdbbj4nnfiyfad4xvlon";

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/21, /* clock=*/18, /* data=*/17);


// Function to display text on the OLED screen
void displayText(const char* text) {
  u8g2.clearBuffer();               // Clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr); // Choose a suitable font
  u8g2.drawStr(0, 10, text);        // Write something to the internal memory
  u8g2.sendBuffer();                // Transfer internal memory to the display
}

void setup() {
  u8g2.begin();
  Serial.begin(115200);
  Serial.println("Booting");
  displayText("Booting...");

  // Initialize WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Initialize OTA
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
  
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();
    if (!client.connect(server, port)) {
      Serial.println("Connection to server failed!");
      return;
    }

    client.print(String("GET ") + path + " HTTP/1.1\r\n" +
                 "Host: " + server + "\r\n" +
                 "Authorization: " + bearer_token + "\r\n" +
                 "Connection: close\r\n\r\n");

    while (client.connected() || client.available()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
          break;
        }
      }
    }

    String jsonResponse = "";
    while (client.available()) {
      String line = client.readStringUntil('\n');
      jsonResponse += line;
    }

    // Print the entire JSON response to Serial
    Serial.println("JSON Response:");
    Serial.println(jsonResponse);

    client.stop();

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, jsonResponse);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    bool onValue = doc["On"];
    Serial.print("On: ");
    Serial.println(onValue);
    displayText("Current state is: " + onValue);
  }

  delay(1000); // Repeatedly Send
}
