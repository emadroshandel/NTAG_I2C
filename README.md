# Example: Write on NTAG via I2C

Read and Write NDEF messages on NTAGs such as NT3H1101, NT3H2211, and NT3H2111 through I2C. 

NFC Data Exchange Format (NDEF) is a common data format that operates across all NFC devices, regardless of the underlying tag or device technology.

This example uses the previously provided libraries in this profile. The required libraries are: 
### Requires
- [MFRC522_I2C_Lib](https://github.com/emadroshandel/MFRC522_I2C_Lib).
- [NDEF_MFRC522_I2C](https://github.com/emadroshandel/NDEF_MFRC522_I2C.git).

### Supports

- Reading from NTAGs over I2C.
- Writing NDEF format to NTAGs over I2C.
- Works on Arduino boards and all other processors which are programmed with arduino. 

### Initialization
- Connect your NTAG module to the microcontroller.
- Flash the code to your processor board using the Arduino IDE.
- Open the Serial Monitor at 115200 baud rate for debugging.

## Read/Write NDEF
Firstly, you need to make the NDEF message using MakeNDEF() function as follow:
This function creates and encodes the message. 
- A URI Record (e.g., a website URL).
- A converted int to string to save it as a text. 
- Text Records (e.g., a numeric value or descriptive text).
- The message is encoded into a byte array (ndefBytes) for writing to NTAG memory.
```
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
```
I have separated the read and write functions to two parts. The main one reads and write over I2C and the other one allows to read from or write on several pages.

### Functions required for reading:
- Function to read a single page:
```
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
```
- Function to read multiple pages:
```
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
```
- Function to write to a single page:
```
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
```
// Function to write to multiple pages with data from a buffer
```
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
```
I have provided a separated function for writing the NDEF message in the Loop. This functions Encodes the NDEF message defined by MakeNDEF() function and writes it to NTAG memory pages.
```
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
```
## License

[MIT License](https://github.com/emadroshandel/NTAG_I2C/blob/main/LICENSE Copyright (c) 2024 Emad Roshandel
