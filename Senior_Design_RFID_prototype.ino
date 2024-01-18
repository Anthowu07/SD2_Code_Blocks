/*
This code currently waits for a "read and execute" button to be pressed.
Once pressed, if there's a tag on the RFID sensor, it will save that instruction in an array and execute shortly after.
After code is executed, queue is cleared and loop ends. 

To do:
* separate read and execute to two different buttons
* make it work with more than one rfid
 */
 
#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 10
#define RST_PIN 7
#define RLED_PIN 3
#define GLED_PIN 2
#define buttonPin 6
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;
int commands[6] = {0, 0, 0, 0, 0, 0};

int blockNum = 2;  
byte bufferLen = 18;
byte readBlockData[18];
 
void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  pinMode(RLED_PIN, OUTPUT);
  pinMode(GLED_PIN, OUTPUT);
  pinMode(buttonPin, INPUT);

  
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  Serial.println("Approximate your card to the reader...");
  Serial.println();
  int commands[6];
}
void loop() 
{
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (digitalRead(buttonPin) == HIGH) {

    while(ReadyToRead()){
      ReadDataFromBlock(blockNum, readBlockData);
      Serial.println(readBlockData[0]);

      if (readBlockData[0] == 49)
      {
        Serial.println("Green LED for 2 seconds");
        Serial.println();
        for(int i = 0; i < 6; i++){
          if(commands[i] == 0){
            commands[i] = 1;
            break;
          }
        }
        delay(3000);
      }
    
      else if (readBlockData[0] == 50)   {
        Serial.println("Red LED for 2 seconds");
        Serial.println();
        for(int i = 0; i < 6; i++){
          if(commands[i] == 0){
            commands[i] = 2;
            break;
          }
        }
        delay(3000);
      }else{
        Serial.println("Not recognized");
      }
    }


    Serial.println("Commands Queue: ");
    for(int i = 0; i < 6; i++){
      Serial.println(commands[i]);
      if(commands[i] == 1){
        digitalWrite(GLED_PIN, HIGH);
        delay(2000);
        digitalWrite(GLED_PIN, LOW);
      }else if (commands[i] == 2){
        digitalWrite(RLED_PIN, HIGH);
        delay(2000);
        digitalWrite(RLED_PIN, LOW);
      }
      commands[i] = 0;
    }
    delay(3000);
  }

  
} 

void ReadDataFromBlock(int blockNum, byte readBlockData[]) 
{
  /* Authenticating the desired data block for Read access using Key A */
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK)
  {
     Serial.print("Authentication failed for Read: ");
     Serial.println(mfrc522.GetStatusCodeName(status));
     return;
  }
  else
  {
    Serial.println("Authentication success");
  }

  /* Reading data from the Block */
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Block was read successfully");

    delay(1000); //change value if you want to read cards faster

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}

boolean ReadyToRead() 
{
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
  return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
  return false;
  }
  String tagID = "";
  for ( uint8_t i = 0; i < 4; i++) { // The MIFARE PICCs that we use have 4 byte UID
  //readCard[i] = mfrc522.uid.uidByte[i];
  tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
  }
  tagID.toUpperCase();
  return true;
}