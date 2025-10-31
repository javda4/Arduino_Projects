#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <RadioLib.h>

const char* ssid = "ORBI25";
const char* password = "greencream614";
const char* server = "api.adalo.com";
const int port = 443;
const char* path = "/v0/apps/614a75c2-7d33-4d68-a979-8e42076d446f/collections/t_5n9x6q85597znb76t9v2t6gjw/1";
const char* bearer_token = "Bearer 0bj9lvdbbj4nnfiyfad4xvlon";

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
  Serial.println("Booting");

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

  // Initialize LoRa module using RadioLib
  SPI.begin(LoRa_SCK, LoRa_MISO, LoRa_MOSI, LoRa_nss);
  int state = radio.begin();
  if (state == -1) {
    Serial.println("LoRa module initialization failed!");
    while (1);
  }
  Serial.println("LoRa module initialized successfully");
  radio.setFrequency(915000000);  // Set LoRa frequency to 915 MHz
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

    // Send binary bit over LoRa using RadioLib
    uint8_t dataToSend = onValue ? 1 : 0;
    int transmitState = radio.transmit(&dataToSend, sizeof(dataToSend));

    if (transmitState < 0) {
      Serial.print("Error transmitting over LoRa: ");
      Serial.println(transmitState);
    } else {
      Serial.println("LoRa transmission successful!");
    }
  }

  delay(60000); // Wait for 1 minute before the next request
}
