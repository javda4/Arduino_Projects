#include <WiFi.h>
#include <HTTPClient.h>

// Define your Wi-Fi credentials
const char* ssid = "your-ssid";
const char* password = "your-password";

// Define the updated URL and the Bearer token for the request
const char* url = "https://api.adalo.com/v0/apps/614a75c2-7d33-4d68-a979-8e42076d446f/collections/t_5n9x6q85597znb76t9v2t6gjw/1";
const char* bearerToken = "Bearer 0bj9lvdbbj4nnfiyfad4xvlon";

// Create an instance of HTTPClient
HTTPClient http;

// Define the interval for sending requests (5 seconds)
const unsigned long interval = 5000; // 5 seconds in milliseconds

// Store the last request time
unsigned long lastRequestTime = 0;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");
  
  // Wait until the ESP32 is connected to Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("Connected to Wi-Fi");
}

void loop() {
  // Get the current time
  unsigned long currentTime = millis();
  
  // Check if the interval has passed since the last request
  if (currentTime - lastRequestTime >= interval) {
    // Make the HTTP GET request
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", bearerToken);

    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      // Request was successful
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response:");
      Serial.println(response);
    } else {
      // Request failed
      Serial.println("HTTP Request failed");
    }
    
    // End the HTTP request
    http.end();
    
    // Update the last request time
    lastRequestTime = currentTime;
  }
}
