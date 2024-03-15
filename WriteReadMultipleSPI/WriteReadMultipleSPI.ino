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

const int buttonPin = 2;

int commands[6] = {0, 0, 0, 0, 0, 0};

void setup(void) {
  
  Serial.begin(9600);

  pinMode(buttonPin, INPUT);

  for (int i = 0; i < READERS; i++) {
    nfcDevices[i].begin();
  }

}

void loop(void) {
  // Configured to only use 1
  if (digitalRead(buttonPin) == HIGH) {
    for (int i = 0; i < READERS; i++) {

      Serial.print("\nScan a NFC tag for reader ");
      Serial.print(i);
      Serial.print("\n");

      if (nfcDevices[i].tagPresent())
      {
        NdefMessage message = NdefMessage();
        message.addUriRecord("Forward");

        bool success = nfcDevices[i].write(message);
        if (success) {
          Serial.println("Write success.");        
        } else {
          Serial.println("Write failed.");
        }

      delay(300);
        byte payload[8];
        NfcTag tag = nfcDevices[i].read();
        tag.getNdefMessage().getRecord(0).getPayload(payload);

        //tag.print();
        //Serial.println(payload, HEX);
        PrintHexChar(payload, sizeof(payload));
        byte forwardArray[] = {0x0 ,'F', 'o', 'r', 'w', 'a', 'r', 'd'};
        if(memcmp(payload, forwardArray, sizeof(payload)) == 0){
          Serial.println("Verified");
        }else{
          Serial.println("Naur");
        }
      }

      delay(1000);
    }
  }
}