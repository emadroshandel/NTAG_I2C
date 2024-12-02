//************************************************
#define MIFARE_ULTRALIGHT_DEBUG
void MakeNDEF(uint8_t *&ndefBytes, int &ndefSize){
    NdefMessage messag = NdefMessage();

    messag.addUriRecord("https://www.ExampleWebPage.com.au");
    messag.addTextRecord(String(150));
    messag.addTextRecord("Example Text");

    // Determine the size and allocate the NDEF byte array
    ndefSize = messag.getEncodedSize();
    Serial.print("NDEF Message Size: ");
    Serial.println(ndefSize);

    ndefBytes = new uint8_t[ndefSize];  // Dynamically allocate memory for the NDEF message bytes
    messag.encode(ndefBytes);           // Encode the message into the byte array
}
//************************************************

// Function to read multiple pages
void readPages(uint8_t startPage, uint8_t endPage) {
    uint8_t buffer[NFC_PAGE_SIZE];
    
    for (uint8_t page = startPage; page <= endPage; page++) {
        if (readPage(page, buffer)) {
            Serial.print("Page ");
            Serial.print(page);
            Serial.print(": ");
            for (int i = 0; i < NFC_PAGE_SIZE; i++) {
                Serial.print(buffer[i], HEX);
                Serial.print(" ");
            }
            Serial.println();
        } else {
            Serial.print("Failed to read page ");
            Serial.println(page);
        }
    }
}

// Function to write to multiple pages with data from a buffer
void writePages(uint8_t startPage, uint8_t endPage, const uint8_t *data, int dataLength) {
    uint8_t buffer[NFC_PAGE_SIZE];
    int index = 0;
    
    for (uint8_t page = startPage; page <= endPage && index < dataLength; page++) {
        for (int i = 0; i < NFC_PAGE_SIZE; i++) {
            if (index < dataLength) {
                buffer[i] = data[index++];
            } else {
                buffer[i] = 0x00; // Fill remaining with 0 if data is insufficient
            }
        }
        
        if (writePage(page, buffer)) {
            Serial.print("Page ");
            Serial.print(page);
            Serial.println(" written successfully.");
        } else {
            Serial.print("Failed to write page ");
            Serial.println(page);
        }
    }
}

// Function to read a single page
bool readPage(uint8_t page, uint8_t *buffer) {
    if (page > USER_END_PAGE) {
        Serial.println("Error: Page out of range.");
        return false;
    }
    
    Wire.beginTransmission(NFC_I2C_ADDRESS);
    Wire.write(page);                  // Set the page address
    if (Wire.endTransmission(false) != 0) {
        Serial.println("Failed to initiate read.");
        return false;
    }
    
    Wire.requestFrom(NFC_I2C_ADDRESS, NFC_PAGE_SIZE);
    if (Wire.available() < NFC_PAGE_SIZE) {
        Serial.println("Failed to read full page.");
        return false;
    }
    
    for (int i = 0; i < NFC_PAGE_SIZE; i++) {
        buffer[i] = Wire.read();
    }
    
    return true;
}

// Function to write to a single page
bool writePage(uint8_t page, const uint8_t *buffer) {
    if (page > USER_END_PAGE) {
        Serial.println("Error: Page out of range.");
        return false;
    }
    
    Wire.beginTransmission(NFC_I2C_ADDRESS);
    Wire.write(page);                  // Set the page address
    Wire.write(buffer, NFC_PAGE_SIZE); // Write 4 bytes to the page
    if (Wire.endTransmission() != 0) {
        Serial.println("Failed to write page.");
        return false;
    }

    delay(5); // Small delay to allow NTAG to process the write
    return true;
}
//****************************************************************************//
// Place in RTC page:
void i2cWriteOnNtag(){
    // Generate the NDEF byte array
    uint8_t *ndefBytes;
    int ndefSize;
    MakeNDEF(ndefBytes, ndefSize);

    Serial.println("Encoded NDEF message:");
    for (int i = 0; i < ndefSize; ++i) {
        Serial.print(ndefBytes[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    // Write the NDEF data to pages 3 to 9
    Serial.println("Writing NDEF message to pages 1 to 15:");
    writePages(1, 15, ndefBytes, ndefSize);

    // Clean up dynamically allocated memory for the NDEF bytes
    delete[] ndefBytes;
}

void i2cReadNtag(){
    Serial.println("Reading pages 1 to 15:");
    readPages(1, 15);
}
//******************************************************************************//
void page0Update(){
  uint8_t page0info[16];
  readPage(0, page0info);
  Serial.println("Original I2C Page 0 data (RF pages 0-3):");
        for (int i = 0; i < 16; i++) {
            Serial.print(page0info[i], HEX);
            Serial.print(" ");
            if ((i + 1) % 4 == 0) Serial.println();
        }
  // Step 2: Modify only the last 4 bytes (RF page 3)
  page0info[12] = 0xE1; 
  page0info[13] = 0x10;
  page0info[14] = 0x6F;
  page0info[15] = 0x00;

}
