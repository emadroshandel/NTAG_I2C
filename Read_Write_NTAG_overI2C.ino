
#include "util/delay.h"
#include <avr/interrupt.h>
#include "avr/io.h"
#include <Wire.h>
#include "MFRC522_I2C.h"
#include <NdefMessage.h>
/*******************************************************************************************************/
using namespace ndef_mfrc522;

#define NFC_I2C_ADDRESS 0x55   // Adjust this if necessary
#define NFC_PAGE_SIZE 4*4      // Each page on NTAG over i2c is 16 bytes. while pages in RF communication are 4 bytes. 
#define USER_START_PAGE 0      // First user memory page
#define USER_END_PAGE 128      // Last user memory page

unsigned long ntagmilis = 0;   // delay for writing on the NTAG
/*******************************************************************************************************/
void setup() {
  pinMode(15, OUTPUT);          // LED
  Serial.begin(115200);
  Wire.begin();
    uint8_t buffer[NFC_PAGE_SIZE];
    if (readPage(USER_START_PAGE, buffer)) {
        Serial.println("NTAG communication successful.");
    } else {
        Serial.println("NTAG communication failed.");
    }

  page0Update();

  _delay_ms(1000);
}

void loop() {
    if (millis() - ntagmilis > 10000) {     // Note that 1000 is the summation of all delays in the entire code
    i2cReadNtag();
    i2cWriteOnNtag();
    ntagmilis = millis();
  }
}
