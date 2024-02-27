
#if 1
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spiONE(SPI, 36);
PN532_SPI pn532spiTWO(SPI, 37);
PN532_SPI pn532spiTHREE(SPI, 38);

NfcAdapter nfc = NfcAdapter(pn532spiONE);
NfcAdapter nfc2 = NfcAdapter(pn532spiTWO);
NfcAdapter nfc3 = NfcAdapter(pn532spiTHREE);
#define READERS 3

#else

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
#endif

void setup(void) {
    Serial.begin(9600);
    Serial.println("NDEF Reader");
    nfc.begin();
    nfc2.begin();
    nfc3.begin();
}

void loop(void) {
    Serial.println("\nScan a NFC tag\n");
    if (nfc.tagPresent())
    {
        NfcTag tag = nfc.read();
        tag.print();
    }

    delay(5000);

    Serial.println("\nScan a NFC tag two\n");
    if (nfc2.tagPresent())
    {
        NfcTag tag = nfc2.read();
        tag.print();
    }

    delay(5000);
        Serial.println("\nScan a NFC tag three\n");
    if (nfc3.tagPresent())
    {
        NfcTag tag = nfc3.read();
        tag.print();
    }
    
    delay(5000);
}