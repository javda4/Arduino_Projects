#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <U8g2lib.h>

const char* ssid = "ORBI25"; //wifi username
const char* password = "greencream614"; //wifi password
const char* server = "api.adalo.com";
const int port = 443;
const char* url_path = "/v0/apps/614a75c2-7d33-4d68-a979-8e42076d446f/collections/t_d31pae6qu475xevmok3fophxd/48";
const char* bearer_token = "Bearer 0bj9lvdbbj4nnfiyfad4xvlon";

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/21, /* clock=*/18, /* data=*/17);

// Relay pin definition
#define RELAY_PIN 5
// Button pin definition
#define BUTTON_PIN 4

bool relayState = false;
bool buttonPressed = false;

// Function to display text on the OLED screen
void displayText(const char* text) {
  u8g2.clearBuffer();               // Clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr); // Choose a suitable font
  u8g2.drawStr(0, 10, text);        // Write something to the internal memory
  u8g2.sendBuffer();                // Transfer internal memory to the display
}

// Function to send the relay state to the server
void postRelayState(bool state) {
  WiFiClientSecure client;
  client.setInsecure();
  if (!client.connect(server, port)) {
    Serial.println("Connection to server failed!");
    return;
  }

  DynamicJsonDocument doc(1024);
  doc["On"] = state;
  String jsonString;
  serializeJson(doc, jsonString);

  client.print(String("POST ") + url_path + " HTTP/1.1\r\n" +
               "Host: " + server + "\r\n" +
               "Authorization: " + bearer_token + "\r\n" +
               "Content-Type: application/json\r\n" +
               "Content-Length: " + jsonString.length() + "\r\n" +
               "Connection: close\r\n\r\n" +
               jsonString);

  while (client.connected() || client.available()) {
    if (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line); // Print server response for debugging
    }
  }

  client.stop();
}

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

  // Initialize relay pin
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Ensure the relay is off initially

  // Initialize button pin
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  u8g2.begin();
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

    client.print(String("GET ") + url_path + " HTTP/1.1\r\n" +
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

    relayState = doc["On"];
    Serial.print("On: ");
    Serial.println(relayState);

    // Control the relay based on the JSON response
    if (relayState) {
      digitalWrite(RELAY_PIN, HIGH); // Activate relay
      displayText("Relay ON");
    } else {
      digitalWrite(RELAY_PIN, LOW); // Deactivate relay
      displayText("Relay OFF");
    }
  }

  // Check if the button is pressed
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!buttonPressed) {
      buttonPressed = true;
      relayState = !relayState; // Toggle relay state
      digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
      postRelayState(relayState);
      displayText(relayState ? "Relay ON" : "Relay OFF");
    }
  } else {
    buttonPressed = false;
  }

  delay(1000); // Repeatedly Send
}
