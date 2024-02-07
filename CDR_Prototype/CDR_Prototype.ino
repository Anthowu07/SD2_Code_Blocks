#include <MFRC522.h>
#include <SPI.h>

/*  
  Three states determined from variable status.



*/

/*
  Anthony Wu 1/31/2024
  Current Functionality:
    1) User presses button 1
    2) If there is a tag on all rfids, a command queue is filled with the codes in each tag in order
    3) user Presses button 2
    4) Commands in the command queue are executed in order
    5) Command queue is completely cleared

  Notes:
    - All sensors must have a tag for anything to be read
    - Sometimes, you must hold down button 1 to read properly
    - Numeber of RFID readers is configurable
    - The queue cannot hold more than 6 commands currently
    - Works for only forward and backwards currently
*/

// Motor driver 1
#define enA 3 // Motor 1 PWM
#define in1 24 // Motor 1
#define in2 22 // Motor 1
#define enB 5 // Motor 2 PWM
#define in3 26 // Motor 2
#define in4 28 //Motor 2

// Motor driver 2
#define enC 6 // Motor 3 PWM
#define in5 25 // Motor 3 in1
#define in6 27 // Motor 3 in2
#define enD 9 // Motor 4 PWM
#define in7 31 // Motor 4 in3
#define in8 33 // Motor 4 in4

#define button 4
#define button2 8
#define trigPin 49
#define echoPin 47

#define RST_PIN   12 
#define SS_PIN    13 // RFID 1
#define SS_PIN2   23 // RFID 2
#define NR_OF_READERS   2
byte ssPins[] = {SS_PIN, SS_PIN2};

long duration;
int distance;
int status = 0; // Controls the status of the motors

// Create instance
MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

int commands[6] = {0, 0, 0, 0, 0, 0};
int blockNum = 2;  
byte bufferLen = 18;
byte readBlockData[18];

int rotDirection = 0;

void setup() {

  // Initiating
  SPI.begin();        // SPI bus
  //mfrc522.PCD_Init(); //  MFRC522
  //mfrc522b.PCD_Init(); //  MFRC522
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();
  }

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600);
  // Motor driver 1
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  // Motor driver 2
  pinMode(enC, OUTPUT);
  pinMode(in5, OUTPUT);
  pinMode(in6, OUTPUT);
  pinMode(enD, OUTPUT);
  pinMode(in7, OUTPUT);
  pinMode(in8, OUTPUT);
  
  pinMode(button, INPUT);
  pinMode(button2, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);


  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  digitalWrite(in5, HIGH);
  digitalWrite(in6, LOW);

  digitalWrite(in7, HIGH);
  digitalWrite(in8, LOW);

  Serial.println("Press Button one to begin read...");
  Serial.println();      
  
}

int pressed = 0;
int toggle = 0;
int speed = 255;
void loop() {
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }

  // Check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  // Begin Read
  if (digitalRead(button) == HIGH) {
    Serial.println("Button Pressed");
    if(ReadyToRead()){                         //Checks if a tag is on all RFIDs
      Serial.println("Scanning...");
      ReadDataFromBlock(blockNum, readBlockData); //Also calls FillsQueue()
    }
    delay(1000);
  }

  // Execute commands in order
  if (digitalRead(button2) == HIGH) {
    Serial.println("Commands Queue: ");
    for(int i = 0; i < 6; i++){
      Serial.println(commands[i]);
      if(commands[i] == 1){
        forwardMove();
      }else if (commands[i] == 2){
        backwardMove();
      }
      commands[i] = 0;
    }
    delay(1000);
  }
}

//Reads data in each tag and fills command queue with the data found in the tags
void ReadDataFromBlock(int blockNum, byte readBlockData[]) 
{
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    /* Authenticating the desired data block for Read access using Key A */
    byte status = mfrc522[reader].PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522[reader].uid));

    if (status != MFRC522::STATUS_OK)
    {
      Serial.print("Authentication failed for Read: ");
      Serial.println(mfrc522[reader].GetStatusCodeName(status));
      return;
    }
    else
    {
      Serial.println("Authentication success");
    }

    /* Reading data from the Block */
    status = mfrc522[reader].MIFARE_Read(blockNum, readBlockData, &bufferLen);
    if (status != MFRC522::STATUS_OK)
    {
      Serial.print("Reading failed: ");
      Serial.println(mfrc522[reader].GetStatusCodeName(status));
      return;
    }
    else
    {
      Serial.println("Block was read successfully");

      delay(1000); //change value if you want to read cards faster

      mfrc522[reader].PICC_HaltA();
      mfrc522[reader].PCD_StopCrypto1();

      Serial.println(readBlockData[0]);

      FillQueue(readBlockData[0]);

    }
  }
}

//Checks if there is a tag on all RFIDs, otherwise ends operation
boolean ReadyToRead() 
{
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    // Getting ready for Reading PICCs
    if ( ! mfrc522[reader].PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    Serial.print("Failed to read ");
    Serial.println(reader);
    return false;
    }
    if ( ! mfrc522[reader].PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
    //Serial.println("Failed to read2");
    return false;
    }
    String tagID = "";
    for ( uint8_t i = 0; i < 4; i++) { // The MIFARE PICCs that we use have 4 byte UID
    //readCard[i] = mfrc522.uid.uidByte[i];
    tagID.concat(String(mfrc522[reader].uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
    }
    tagID.toUpperCase();
  }
  return true;
}

//Fills command Queue with data read from ReadDataFromBlock()
void FillQueue(byte readBlockData){
  if (readBlockData == 49)
  {
    Serial.println("Move Forward");
    Serial.println();
    for(int i = 0; i < 6; i++){
      if(commands[i] == 0){
        commands[i] = 1;
        break;
      }
    }
    delay(300);
  }
    
  else if (readBlockData == 50)   {
    Serial.println("Move Backward");
    Serial.println();
    for(int i = 0; i < 6; i++){
      if(commands[i] == 0){
        commands[i] = 2;
        break;
      }
    }
    delay(300);
  }else{
    Serial.println("Not recognized");
  }
}
 
void readUltrasonic() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  //Serial.print("Distance: ");
  //Serial.println(distance);
  if (distance < 5) {
    status = -1;
  }
}

// Forward movement for a set time. 
void forwardMove() {
  analogWrite(enA, 255); // Send PWM signal to L298N Enable pin
  analogWrite(enB, 255); // Send PWM signal to L298N Enable pin
  analogWrite(enC, 255); // Send PWM signal to L298N Enable pin
  analogWrite(enD, 255); // Send PWM signal to L298N Enable pin

  // Four second delay
  delay(4000);

  stopMove();
}

void resetDirection() {
  // Set back to normal movement
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  digitalWrite(in5, HIGH);
  digitalWrite(in6, LOW);
  digitalWrite(in7, HIGH);
  digitalWrite(in8, LOW); 
}

// Backward movement for a set time. 
void backwardMove() {

  // Inverse in pin values to change to backwards movement
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  digitalWrite(in5, LOW);
  digitalWrite(in6, HIGH);
  digitalWrite(in7, LOW);
  digitalWrite(in8, HIGH); 
  
  analogWrite(enA, 255); // Send PWM signal to L298N Enable pin
  analogWrite(enB, 255); // Send PWM signal to L298N Enable pin
  analogWrite(enC, 255); // Send PWM signal to L298N Enable pin
  analogWrite(enD, 255); // Send PWM signal to L298N Enable pin

  // Four second delay
  delay(4000);

  stopMove();
  resetDirection();
  
}

/* Assuming
 *  a|----|b
 *   | ^^ |
 *   |    |
 *  d|----|c
 *  in1,2=a
 *  in3,4=b
 *  in5,6=c
 *  in7,8=d
 */
void rotateLeft() {
  // a backwards
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  // d backwards
  digitalWrite(in7, LOW);
  digitalWrite(in8, HIGH);

  // c forwards
  digitalWrite(in5, HIGH);
  digitalWrite(in6, LOW);
  // b forwards
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  // Activate a and c at full speed
  analogWrite(enA, 255); // Send PWM signal to L298N Enable 
  analogWrite(enB, 255); // Send PWM signal to L298N Enable 
  analogWrite(enC, 255); // Send PWM signal to L298N Enable 
  analogWrite(enD, 255); // Send PWM signal to L298N Enable 

  // 1000 = 1 second.
  delay(3000);

  stopMove();
  resetDirection();
}


void rotateRight() {
  
  // d forward
  digitalWrite(in7, HIGH);
  digitalWrite(in8, LOW);
  // a forward
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
  // b backwards
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  // c backwards
  digitalWrite(in5, LOW);
  digitalWrite(in6, HIGH);
  
  analogWrite(enA, 255); // Send PWM signal to L298N Enable 
  analogWrite(enB, 255); // Send PWM signal to L298N Enable
  analogWrite(enC, 255); // Send PWM signal to L298N Enable 
  analogWrite(enD, 255); // Send PWM signal to L298N Enable 
   
  // 1000 = 1 second.
  delay(3000);
  
  stopMove();
  resetDirection();
}

/* F = forward, R = reversed
 * F a|----|b R
 *    | ^^ |
 *    |    |
 * R d|----|c F
 *  in1,2=a
 *  in3,4=b
 *  in5,6=c
 *  in7,8=d
 */
void sidewayRight() {

  // a forward
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  // b backwards
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  // d backwards
  digitalWrite(in7, LOW);
  digitalWrite(in8, HIGH);
  // c forwards
  digitalWrite(in5, HIGH);
  digitalWrite(in6, LOW);

  analogWrite(enA, 255); // Send PWM signal to L298N Enable 
  analogWrite(enB, 255); // Send PWM signal to L298N Enable
  analogWrite(enC, 255); // Send PWM signal to L298N Enable 
  analogWrite(enD, 255); // Send PWM signal to L298N Enable 

  delay(4000);

  stopMove();
  resetDirection();
}

/* F = forward, R = reversed
 * R a|----|b F
 *    | ^^ |
 *    |    |
 * F d|----|c R
 *  in1,2=a
 *  in3,4=b
 *  in5,6=c
 *  in7,8=d
 */
void sidewayLeft() {

  // a backwards
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  // b forwards
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  // d forwards
  digitalWrite(in7, HIGH);
  digitalWrite(in8, LOW);
  // c backwards
  digitalWrite(in5, LOW);
  digitalWrite(in6, HIGH);

  analogWrite(enA, 255); // Send PWM signal to L298N Enable 
  analogWrite(enB, 255); // Send PWM signal to L298N Enable
  analogWrite(enC, 255); // Send PWM signal to L298N Enable 
  analogWrite(enD, 255); // Send PWM signal to L298N Enable 

  delay(4000);

  stopMove();
  resetDirection();
}

/* F = forward, R = reversed
 * F a|----|b 
 *    | ^^ |
 *    |    |
 *   d|----|c F
 *  in1,2=a
 *  in3,4=b
 *  in5,6=c
 *  in7,8=d
 */
void diagonalForwardRight() {

  // a forward
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  // c forwards
  digitalWrite(in5, HIGH);
  digitalWrite(in6, LOW);

  analogWrite(enA, 255); // Send PWM signal to L298N Enable 
  analogWrite(enC, 255); // Send PWM signal to L298N Enable 

  delay(4000);

  stopMove();
  resetDirection();
}

/* F = forward, R = reversed
 *   a|----|b  F
 *    | ^^ |
 *    |    |
 * F d|----|c 
 *  in1,2=a
 *  in3,4=b
 *  in5,6=c
 *  in7,8=d
 */
void diagonalForwardLeft() {

  // b forwards
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  // d forwards
  digitalWrite(in7, HIGH);
  digitalWrite(in8, LOW);

  analogWrite(enB, 255); // Send PWM signal to L298N Enable
  analogWrite(enD, 255); // Send PWM signal to L298N Enable 

  delay(4000);

  stopMove();
  resetDirection();
}

/* F = forward, R = reversed
 *   a|----|b R
 *    | ^^ |
 *    |    |
 * R d|----|c 
 *  in1,2=a
 *  in3,4=b
 *  in5,6=c
 *  in7,8=d
 */
void diagonalBackwardRight() {
  // b backwards
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  // d backwards
  digitalWrite(in7, LOW);
  digitalWrite(in8, HIGH);

  analogWrite(enB, 255); // Send PWM signal to L298N Enable
  analogWrite(enD, 255); // Send PWM signal to L298N Enable

  delay(4000);

  stopMove();
  resetDirection();
}

/* F = forward, R = reversed
 * R a|----|b  
 *    | ^^ |
 *    |    |
 *   d|----|c R
 *  in1,2=a
 *  in3,4=b
 *  in5,6=c
 *  in7,8=d
 */
void diagonalBackwardLeft() {
  // a backwards
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  // c backwards
  digitalWrite(in5, LOW);
  digitalWrite(in6, HIGH);

  analogWrite(enA, 255); // Send PWM signal to L298N Enable 
  analogWrite(enC, 255); // Send PWM signal to L298N Enable 

  delay(4000);

  stopMove();
  resetDirection();
}

void stopMove() {
  analogWrite(enA, 0); // Send turn off signal
  analogWrite(enB, 0); // Send turn off signal
  analogWrite(enC, 0); // Send turn off signal
  analogWrite(enD, 0); // Send turn off signal
} 
