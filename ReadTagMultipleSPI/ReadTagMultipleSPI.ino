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

#define READERS 7
NfcAdapter nfcDevices[READERS] = {NfcAdapter(pn532spiONE), NfcAdapter(pn532spiTWO), NfcAdapter(pn532spiTHREE), 
NfcAdapter(pn532spiFOUR), NfcAdapter(pn532spiFIVE), NfcAdapter(pn532spiSIX), NfcAdapter(pn532spiSEVEN)};
/*
nfcDevices[0] = NfcAdapter(pn532spiONE);
nfcDevices[1] = NfcAdapter(pn532spiTWO);
nfcDevices[2] = NfcAdapter(pn532spiTHREE);
nfcDevices[3] = NfcAdapter(pn532spiFOUR);
nfcDevices[4] = NfcAdapter(pn532spiFIVE);
nfcDevices[5] = NfcAdapter(pn532spiSIX);
nfcDevices[6] = NfcAdapter(pn532spiSEVEN);*/

void setup(void) {



    Serial.begin(9600);
    Serial.println("NDEF Reader");

    for (int i = 0; i < READERS; i++) {
      nfcDevices[i].begin();
    }
    /*
    nfc.begin();
    nfc2.begin();
    nfc3.begin();
    nfc4.begin();
    nfc5.begin();
    nfc6.begin();
    nfc7.begin();*/
}

void loop(void) {
  for (int i = 0; i < READERS; i++) {

    Serial.print("\nScan a NFC tag for reader ");
    Serial.print(i);
    Serial.print("\n");

    if (nfcDevices[i].tagPresent())
    {
        NfcTag tag = nfcDevices[i].read();
        tag.print();
    }

    delay(5000);
  }
}
