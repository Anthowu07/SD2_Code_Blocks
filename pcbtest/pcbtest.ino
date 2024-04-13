#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spiONE(SPI, 5);
PN532_SPI pn532spiTWO(SPI, 3);
PN532_SPI pn532spiTHREE(SPI, 4);
PN532_SPI pn532spiFOUR(SPI, 6);
PN532_SPI pn532spiFIVE(SPI, 7);
PN532_SPI pn532spiSIX(SPI, 2);

#define READERS 6
//NfcAdapter nfcDevices[READERS] = {NfcAdapter(pn532spiONE), NfcAdapter(pn532spiTWO), NfcAdapter(pn532spiTHREE), 
//NfcAdapter(pn532spiFOUR), NfcAdapter(pn532spiFIVE), NfcAdapter(pn532spiSIX), NfcAdapter(pn532spiSEVEN)};
NfcAdapter nfcDevices[READERS] = {NfcAdapter(pn532spiONE), NfcAdapter(pn532spiTWO), NfcAdapter(pn532spiTHREE), 
NfcAdapter(pn532spiFOUR), NfcAdapter(pn532spiFIVE), NfcAdapter(pn532spiSIX)};
int commandsQ[7] = {0, 0, 0, 0, 0, 0, 0};
int executedQ[7] = {0, 0, 0, 0, 0, 0, 0};
byte leds = 0;

byte forwardArray[] = {0x0 ,'F', 'o', 'r', 'w', 'a', 'r', 'd'};
byte backwardArray[] = {0x0 ,'B', 'a', 'c', 'k', 'w', 'r', 'd'};
byte rotateLeftArray[] = {0x0 ,'R', 'o', 't', 'a', 't', 'e', 'L'};
byte rotateRightArray[] = {0x0 ,'R', 'o', 't', 'a', 't', 'e', 'R'};
byte sidewaysRightArray[] = {0x0 ,'S', 'i', 'd', 'e', 'w', 'y', 'R'};
byte sidewaysLeftArray[] = {0x0 ,'S', 'i', 'd', 'e', 'w', 'y', 'L'};
byte forwardRightArray[] = {0x0 ,'F', 'w', 'd', 'r', 'g', 'h', 't'};
byte backwardRightArray[] = {0x0 ,'B', 'w', 'd', 'r', 'g', 'h', 't'};
byte backwardLeftArray[] = {0x0 ,'B', 'w', 'd', 'l', 'e', 'f', 't'};
byte forwardLeftArray[] = {0x0 ,'F', 'w', 'd', 'l', 'e', 'f', 't'};
byte stopMoveArray[] = {0x0 ,'S', 't', 'o', 'p', 'm', 'o', 'v'};
byte repeatLastArray[] = {0x0 ,'R', 'e', 'p', 'e', 'a', 't', 'l'};
byte repeatAllArray[] = {0x0 ,'R', 'e', 'p', 'e', 'a', 't', 'a'};


void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
  Serial.println("\n---------------------------------------------------------");
  for (int i = 0; i < READERS; i++) {
    nfcDevices[i].begin();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  bool recognized = true;

  //When the read button is pushed
  
  int index = 0;
  for (int i = 0; i < READERS; i++) {
    Serial.print("\n Scanning RFID ");
    Serial.print(i+1);
    Serial.print("\n");
    commandsQ[i] = 0;
    if (nfcDevices[i].tagPresent())
    {
      
      // //Write instruction into tag
      // NdefMessage message = NdefMessage();
      // message.addUriRecord("Repeata");

      // bool success = nfcDevices[i].write(message);
      // if (success) {
      //   Serial.println("Write success.");        
      // } else {
      //   Serial.println("Write failed.");
      // }
      // delay(300);

      //Read tag, obtain Payload, print payload
      byte payload[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
      NfcTag tag = nfcDevices[i].read();
      tag.getNdefMessage().getRecord(0).getPayload(payload);
      PrintHexChar(payload, sizeof(payload));

      if(memcmp(payload, forwardArray, sizeof(payload)) == 0){
        Serial.println("Forward");
        commandsQ[index] = 1;
      }else if(memcmp(payload, backwardArray, sizeof(payload)) == 0){
        Serial.println("Backward");
        commandsQ[index] = 2;
      }else if(memcmp(payload, rotateLeftArray, sizeof(payload)) == 0){
        Serial.println("Rotate Left");
        commandsQ[index] = 3;
      }else if(memcmp(payload, rotateRightArray, sizeof(payload)) == 0){
        Serial.println("Rotate Right");
        commandsQ[index] = 4;
      }else if(memcmp(payload, sidewaysRightArray, sizeof(payload)) == 0){
        Serial.println("Sideways Right");
        commandsQ[index] = 5;
      }else if(memcmp(payload, sidewaysLeftArray, sizeof(payload)) == 0){
        Serial.println("Sideways Left");
        commandsQ[index] = 6;
      }else if(memcmp(payload, forwardRightArray, sizeof(payload)) == 0){
        Serial.println("Forward Right");
        commandsQ[index] = 7;
      }else if(memcmp(payload, backwardRightArray, sizeof(payload)) == 0){
        Serial.println("Backward Right");
        commandsQ[index] = 8;
      }else if(memcmp(payload, backwardLeftArray, sizeof(payload)) == 0){
        Serial.println("Backward Left");
        commandsQ[index] = 9;
      }else if(memcmp(payload, forwardLeftArray, sizeof(payload)) == 0){
        Serial.println("Forward Left");
        commandsQ[index] = 10;
      }else if(memcmp(payload, stopMoveArray, sizeof(payload)) == 0){
        Serial.println("Stop Moving");
        commandsQ[index] = 11;
      }else if(memcmp(payload, repeatLastArray, sizeof(payload)) == 0){
        Serial.println("Repeat Last");
        commandsQ[index] = 12;
      }else if(memcmp(payload, repeatAllArray, sizeof(payload)) == 0){
        Serial.println("Repeat All");
        commandsQ[index] = 13;
      }else{
        Serial.println("Not recognized");
        recognized = false;
      }

      //If read is succesful, turn on LED
      if(recognized){
        index++;
      }
    }
  }
}
