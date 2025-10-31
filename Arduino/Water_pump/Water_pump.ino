#include <MKRNB.h>
#include <ArduinoHttpClient.h>

// Replace these with your actual APN and PIN
const char APN[] = "simbase";
const char PIN[] = "";

NBSSLClient client;
GPRS gprs;
NB nbAccess;

const char webhookUrl[] = "https://hook.us1.make.com/brrrymo46a8mi5ps4in9r2l3ls46elc5";

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Starting Arduino web client.");

  boolean connected = false;

  while (!connected) {
    if ((nbAccess.begin(APN, PIN) == NB_READY) &&
        (gprs.attachGPRS() == GPRS_READY)) {
      connected = true;
      Serial.println("Connected to GPRS network");
    } else {
      Serial.println("Failed to connect. Retrying...");
      delay(1000);
    }
  }

  Serial.println("Connected to GPRS network.");

  // Perform HTTP POST request to the webhook
  sendDataToWebhook();
}

void loop() {
  // Additional tasks can be added here
}

void sendDataToWebhook() {
  Serial.println("Sending data to webhook...");

  String payload = "{\"key\":\"value\"}";  // Replace with your data

  if (client.connect("hook.us1.make.com", 443)) {
    Serial.println("Connected to webhook server");

    client.print("POST /brrrymo46a8mi5ps4in9r2l3ls46elc5 HTTP/1.1\r\n");
    client.print("Host: hook.us1.make.com\r\n");
    client.print("Content-Type: application/json\r\n");
    client.print("Content-Length: " + String(payload.length()) + "\r\n");
    client.print("\r\n");
    client.print(payload);

    Serial.println("Request sent to webhook");

    delay(1000); // Give some time for the server to respond (adjust as needed)

    // Read and print the response from the server
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
    }

    Serial.println();
    Serial.println("disconnecting from webhook.");
    client.stop();
  } else {
    Serial.println("Failed to connect to webhook");
  }
}
