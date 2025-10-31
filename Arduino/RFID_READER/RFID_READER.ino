#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>


// RFID Pins
#define RST_PIN 5 // Configurable, see pin layout
#define SS_PIN 53 // Configurable, see pin layout

// Initialize the RFID reader
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
MFRC522::MIFARE_Key key;

// LCD Pins
const int rs = 12, en = 11, d4 = 6, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// ASCII for "PJS&Co."
const byte expectedDataBlock1[] = {0x50, 0x4A, 0x53, 0x26, 0x43, 0x6F, 0x2E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};



struct Product {
  const char *name;
  double price;
};

Product products[] = {
  {"Apple", 0.99}, // 1
  {"Banana", 0.69}, // 2
  {"Orange", 1.29}, // 3
  {"Milk", 2.49}, // 4
  {"Bread", 1.99}, // 5
  {"Water Bottle", 1.00}, // 6
  {"Headphones", 100.00}, // 7
  {"Iphone", 739.99}, // 8
  {"Cheetos", 2.99}, // 9
  {"Sweatshirt", 24.99} // 10
};

void displayProductInfo(const String &productName) {
  for (int i = 0; i < sizeof(products) / sizeof(products[0]); i++) {
    if (productName.equalsIgnoreCase(products[i].name)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(products[i].name);
      
      lcd.setCursor(0, 1);
      lcd.print("$" + String(products[i].price, 2));
      
      delay(5000);
      lcd.clear();
      lcd.print("Ready to scan...");
      return;
    }
  }
  lcd.clear();
  lcd.print("Product not");
  lcd.setCursor(0,1);
  lcd.print("found");
  delay(5000);
  lcd.clear();
  lcd.print("Ready to scan...");
}




void setup() {
  // Initialize Serial Monitor and LCD
  Serial.begin(9600); 
  lcd.begin(16, 2);
  lcd.print("Initializing...");

  // Wait for Serial port to connect (for boards like Leonardo)
  while (!Serial); 

  // Initialize SPI bus and RFID reader
  SPI.begin(); 
  mfrc522.PCD_Init(); 

  // Set up the key for authentication (default key is FFFFFFFFFFFFh)
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  lcd.clear();
  lcd.print("Ready to scan...");
  Serial.println(F("Place a card on the reader..."));
}

void loop() {
  // Check if a new card is present
  if (!mfrc522.PICC_IsNewCardPresent()) 
    return;

  // Select the card
  if (!mfrc522.PICC_ReadCardSerial()) 
    return;

  // Authenticate and check for "PJS&Co." in sector 0, block 1
  byte sector = 0;
  byte block1 = 1;
  byte buffer[18];
  byte size = sizeof(buffer);
  MFRC522::StatusCode status;

  // Authenticate using key A
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block1, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed for block 1: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    lcd.clear();
    lcd.print("Key A!");
    delay(3000); // Show message for 3 seconds
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return;
  }

  // Read data from sector 0, block 1
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(block1, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Read failed for block 1: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    lcd.clear();
    lcd.print("Scanning Error");
    delay(5000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ready to scan...");
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return;
  }

  // Check if block 1 data matches "PJS&Co."
  bool isPJS = true;
  for (byte i = 0; i < 16; i++) {
    if (buffer[i] != expectedDataBlock1[i]) {
      isPJS = false;
      break;
    }
  }

  if (!isPJS) {
    Serial.println(F("Block 1 does not contain 'PJS&Co.'"));
    lcd.clear();
    lcd.print("Not PJS&Co.");
    lcd.setCursor(0, 1);
    lcd.print("produce");
    delay(5000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ready to scan...");
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return;
  }

  Serial.println(F("Block 1 verified as 'PJS&Co.'"));
  lcd.clear();
  lcd.print("PJS&Co. Verified");

  // Authenticate and read sector 0, block 2 for product data
  byte block2 = 2;
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block2, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed for block 2: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    lcd.clear();
    lcd.print("Scanning error");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ready to scan...");
    delay(3000);
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return;
  }

  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(block2, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Read failed for block 2: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    lcd.clear();
    lcd.print("Scanning error");
    delay(5000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ready to scan...");
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return;
  }

  // Convert block 2 data from ASCII to a string
  String productName = "";
  for (byte i = 0; i < 16; i++) {
    if (buffer[i] >= 32 && buffer[i] <= 126) { // Printable ASCII range
      productName += (char)buffer[i];
    }
  }




///////////////////////////////////////////////////////////////////////////////////



  // Remove excess whitespace from the product name
  productName.trim();

  Serial.print(F("Product name from block 2: "));
  Serial.println(productName);
  displayProductInfo(productName);
    

  // Halt PICC and stop encryption
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
