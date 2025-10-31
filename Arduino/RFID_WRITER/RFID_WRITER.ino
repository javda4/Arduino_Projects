#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>


#define RST_PIN         5           // Configurable, see typical pin layout above
#define SS_PIN          53          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;


const int rs = 12, en = 11, d4 = 6, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// Change this variable to the product name you want to store
const char* productName = "Nuclear Reactor";

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.print("Initializing..."); 
    while (!Serial);
    SPI.begin();        
    mfrc522.PCD_Init(); 

    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
    lcd.clear();
    lcd.print("Ready to scan...");
    Serial.println(F("Scan a MIFARE Classic PICC to write data to Sector 0, Blocks 1 and 2."));
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent())
        return;

    if (!mfrc522.PICC_ReadCardSerial())
        return;

    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();

    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.print(F("PICC type: "));
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    if (piccType != MFRC522::PICC_TYPE_MIFARE_1K) {
        Serial.println(F("This sample only works with MIFARE Classic 1K cards."));
        return;
    }

    byte sector = 0;
    byte block1 = 1;
    byte block2 = 2;
    byte trailerBlock = 3; // The sector trailer for sector 0 is block 3
    MFRC522::StatusCode status;

    // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Data to write to block 1 (ASCII for "PJS&Co.")
    byte block1Data[16] = { 
        0x50, 0x4A, 0x53, 0x26, 
        0x43, 0x6F, 0x2E, 0x00, 
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00 
    };
    // Creating non-company items
    // byte block1Data[16] = { 
    //     0x00, 0x00, 0x00, 0x00, 
    //     0x00, 0x00, 0x00, 0x00, 
    //     0x00, 0x00, 0x00, 0x00, 
    //     0x00, 0x00, 0x00, 0x00 
    // };


    // Write to block 1
    Serial.println(F("Writing 'PJS&Co.' to Sector 0, Block 1..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(block1, block1Data, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    } else {
        Serial.println(F("Successfully wrote to Block 1!"));
    }

    // Convert the product name into ASCII bytes and pad it to 16 bytes
    byte block2Data[16] = {0};
    for (byte i = 0; i < 16; i++) {
        if (i < strlen(productName)) {
            block2Data[i] = productName[i];
        } else {
            block2Data[i] = 0x00; // Pad with null characters
        }
    }

    // Write to block 2
    Serial.print(F("Writing product name '"));
    Serial.print(productName);
    Serial.println(F("' to Sector 0, Block 2..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(block2, block2Data, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    } else {
        Serial.println(F("Successfully wrote to Block 2!"));
        lcd.clear();
        lcd.print("Successful Write!");
        lcd.setCursor(0, 1);
        lcd.print(productName);
    }

    // Halt the card and stop encryption
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
