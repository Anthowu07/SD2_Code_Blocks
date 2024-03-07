
#if 1
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spiONE(SPI, 36);
PN532_SPI pn532spiTWO(SPI, 37);
PN532_SPI pn532spiTHREE(SPI, 38);
PN532_SPI pn532spiFOUR(SPI, 43);
PN532_SPI pn532spiFIVE(SPI, 41);
PN532_SPI pn532spiSIX(SPI, 40);
PN532_SPI pn532spiSEVEN(SPI, 35);

NfcAdapter nfc = NfcAdapter(pn532spiONE);
NfcAdapter nfc2 = NfcAdapter(pn532spiTWO);
NfcAdapter nfc3 = NfcAdapter(pn532spiTHREE);
NfcAdapter nfc4 = NfcAdapter(pn532spiFOUR);
NfcAdapter nfc5 = NfcAdapter(pn532spiFIVE);
NfcAdapter nfc6 = NfcAdapter(pn532spiSIX);
NfcAdapter nfc7 = NfcAdapter(pn532spiSEVEN);
#define READERS 7

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
    nfc4.begin();
    nfc5.begin();
    nfc6.begin();
    nfc7.begin();
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

            Serial.println("\nScan a NFC tag four\n");
    if (nfc4.tagPresent())
    {
        NfcTag tag = nfc4.read();
        tag.print();
    }
    
    delay(5000);
    
            Serial.println("\nScan a NFC tag five\n");
    if (nfc5.tagPresent())
    {
        NfcTag tag = nfc5.read();
        tag.print();
    }
    
    delay(5000);
    
            Serial.println("\nScan a NFC tag six\n");
    if (nfc6.tagPresent())
    {
        NfcTag tag = nfc6.read();
        tag.print();
    }
    
    delay(5000);
    
            Serial.println("\nScan a NFC tag seven\n");
    if (nfc7.tagPresent())
    {
        NfcTag tag = nfc7.read();
        tag.print();
    }
    
    delay(5000);
    
}
