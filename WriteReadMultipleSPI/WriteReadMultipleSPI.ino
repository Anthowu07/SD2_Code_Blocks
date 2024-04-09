/*  3/18/2024
This code reads from RFID tags when button 1 is pressed. The codes are saved into a queue. The respective LED will light up if something is read. It will stay off if nothing is there or the tag cannot be recognized.
When button 2 is pressed, it reads the queue and executes the movement functions one by one and then clears the queue. The queue is also reset at the start of a read phase.*/

#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

// Motor driver 1
#define enA 3 // Motor 1 PWM
#define in1 25 // Motor 1
#define in2 24 // Motor 1
#define enB 5 // Motor 2 PWM
#define in3 23 // Motor 2
#define in4 22 //Motor 2

// Motor driver 2
#define enC 9 // Motor 3 PWM
#define in5 26 // Motor 3 in1
#define in6 27 // Motor 3 in2
#define enD 6 // Motor 4 PWM
#define in7 28 // Motor 4 in3
#define in8 29 // Motor 4 in4

//74HC595 LED Controller Pins
#define latchPin 12
#define clockPin 13
#define dataPin 4

//Button Pins
#define button1Pin 2
#define button2Pin 8

//Piezo Pin
#define buzzerPin 10

//Ultrasonic Sensors Pins
#define trigPin 20
#define echoPin 21

//PN532 RFID SPI Pins

PN532_SPI pn532spiONE(SPI, 38);
PN532_SPI pn532spiTWO(SPI, 37);
PN532_SPI pn532spiTHREE(SPI, 36);
PN532_SPI pn532spiFOUR(SPI, 43);
PN532_SPI pn532spiFIVE(SPI, 41);
PN532_SPI pn532spiSIX(SPI, 40);
PN532_SPI pn532spiSEVEN(SPI, 35);

#define READERS 7
NfcAdapter nfcDevices[READERS] = {NfcAdapter(pn532spiONE), NfcAdapter(pn532spiTWO), NfcAdapter(pn532spiTHREE), 
NfcAdapter(pn532spiFOUR), NfcAdapter(pn532spiFIVE), NfcAdapter(pn532spiSIX), NfcAdapter(pn532spiSEVEN)};

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

void setup(void) {
  
  Serial.begin(9600);

  for (int i = 0; i < READERS; i++) {
    nfcDevices[i].begin();
  }

  motorSetup();
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
}

void motorSetup() {
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

  // Set initial rotation direction
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  digitalWrite(in5, HIGH);
  digitalWrite(in6, LOW);

  digitalWrite(in7, HIGH);
  digitalWrite(in8, LOW);
}

void loop(void) {
  leds = 0;
  updateShiftRegister();
  bool recognized = true;

  //When the read button is pushed
  if (digitalRead(button1Pin) == HIGH) {
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
        // message.addUriRecord("Fwdrght");

        // bool success = nfcDevices[i].write(message);
        // if (success) {
        //   Serial.println("Write success.");        
        // } else {
        //   Serial.println("Write failed.");
        // }
        //delay(300);

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
          bitSet(leds, i);
          updateShiftRegister();
          index++;
          tone(buzzerPin, 500, 100);
          delay(100);
          tone(buzzerPin, 3000, 200);
          delay(200);
        }else{
        tone(buzzerPin, 3000, 100);
        delay(100);
        tone(buzzerPin, 500, 200);
        delay(200);
      }
        
      }else{
        tone(buzzerPin, 3000, 100);
        delay(100);
        tone(buzzerPin, 500, 200);
        delay(200);
      }
    }
    playEndTune();
  }

  //When execute button is pressed
  if (digitalRead(button2Pin) == HIGH) {
    executeCommands();
  }
}

// Forward movement for a set time. 
void forwardMove() {
  analogWrite(enA, 255); // Send PWM signal to L298N Enable pin
  analogWrite(enB, 255); // Send PWM signal to L298N Enable pin
  analogWrite(enC, 255); // Send PWM signal to L298N Enable pin
  analogWrite(enD, 255); // Send PWM signal to L298N Enable pin
  // for(int i = 0; i < 100000; i++){
  //   readUltrasonic();
  // }
  
  //Four second delay
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
  delay(2800);

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
  delay(2815);
  
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

  delay(5000);

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

  delay(5000);

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

  delay(5000);

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

  delay(5000);

  stopMove();
  resetDirection();
}

void stopMove() {
  analogWrite(enA, 0); // Send turn off signal
  analogWrite(enB, 0); // Send turn off signal
  analogWrite(enC, 0); // Send turn off signal
  analogWrite(enD, 0); // Send turn off signal
}

void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}

void executeCommands(){ //Executes commands in queue one by one and calls the respective function, clears queue after execution
  Serial.println("-------------------Executing-------------------------");
    /*
        Foward: 1
        Backward: 2
        Rotate Left: 3
        Rotate Right: 4
        Sideways Right: 5
        Sideway Left: 6
        Forward Right: 7
        Backward Right: 8
        Backward Left: 9
        Forward Left: 10
        Stop Moving: 11
        Repeat Last: 12
        Repeat All: 13
    */
  for (int i = 0; i < 7; i++) {
    bitSet(leds, i);
    updateShiftRegister();
    switch (commandsQ[i]) {
      case 0:
        Serial.println("Empty");
        break;
      case 1:
        Serial.println("Forward");
        forwardMove();
        executedQ[i] = commandsQ[i];
        commandsQ[i] = 0;
        break;
      case 2:
        Serial.println("Backward");
        backwardMove();
        executedQ[i] = commandsQ[i];
        commandsQ[i] = 0;
        break;
      case 3:
        Serial.println("Rotate Left");
        rotateLeft();
        executedQ[i] = commandsQ[i];
        commandsQ[i] = 0;
        break;
      case 4:
        Serial.println("Rotate Right");
        rotateRight();
        executedQ[i] = commandsQ[i];
        commandsQ[i] = 0;
        break;
      case 5:
        Serial.println("Sideways Right");
        sidewayRight();
        executedQ[i] = commandsQ[i];
        commandsQ[i] = 0;
        break;
      case 6:
        Serial.println("Sideways Left");
        sidewayLeft();
        executedQ[i] = commandsQ[i];
        commandsQ[i] = 0;
        break;
      case 7:
        Serial.println("Forward Right");
        diagonalForwardRight();
        executedQ[i] = commandsQ[i];
        commandsQ[i] = 0;
        break;
      case 8:
        Serial.println("Backward Right");
        diagonalBackwardRight();
        executedQ[i] = commandsQ[i];
        commandsQ[i] = 0;
        break;
      case 9:
        Serial.println("Backward Left");
        diagonalBackwardLeft();
        executedQ[i] = commandsQ[i];
        commandsQ[i] = 0;
        break;
      case 10:
        Serial.println("Forward Left");
        diagonalForwardLeft();
        executedQ[i] = commandsQ[i];
        commandsQ[i] = 0;
        break;
      case 11:
        Serial.println("Stop Moving");
        stopMove();
        executedQ[i] = commandsQ[i];
        commandsQ[i] = 0;
        break;
      case 12:
        Serial.println("Repeat Last");
        if(i == 0){
          Serial.println("Nothing to Repeat");
        }else{
          executeRepeats(i-1);
        }
        //executedQ[i] = commandsQ[i];
        commandsQ[i] = 0;
      case 13:
        Serial.println("Repeat All");
        if(i == 0){
          Serial.println("Nothing to Repeat");
        }else{
          executeRepeats(0);
        }
        //executedQ[i] = commandsQ[i];
        commandsQ[i] = 0;
      default:
        Serial.println("Command not recognized");
        break;
    }
    delay(1000);
  }
  for(int i = 0; i < 7; i++){ //Clear executed queue once all commands have been executed
    executedQ[i] = 0;
  }
}

void executeRepeats(int index){ //Executes comands in the executed queue, if repeat all start at start of queue, if repeat last start at end of queue
  for (int i = index; i < 7; i++) {
    switch (executedQ[i]) {
      case 0:
        Serial.println("Empty");
        break;
      case 1:
        Serial.println("Forward");
        forwardMove();
        break;
      case 2:
        Serial.println("Backward");
        backwardMove();
        break;
      case 3:
        Serial.println("Rotate Left");
        rotateLeft();
        break;
      case 4:
        Serial.println("Rotate Right");
        rotateRight();
        break;
      case 5:
        Serial.println("Sideways Right");
        sidewayRight();
        break;
      case 6:
        Serial.println("Sideways Left");
        sidewayLeft();
        break;
      case 7:
        Serial.println("Forward Right");
        diagonalForwardRight();
        break;
      case 8:
        Serial.println("Backward Right");
        diagonalBackwardRight();
        break;
      case 9:
        Serial.println("Backward Left");
        diagonalBackwardLeft();
        break;
      case 10:
        Serial.println("Forward Left");
        diagonalForwardLeft();
        break;
      case 11:
        Serial.println("Stop Moving");
        stopMove();
        break;
      // case 12:
      //   Serial.println("Repeat Last");
      //   executeRepeats(1, i-1);
      //   //executedQ[i] = commandsQ[i];
      //   commandsQ[i] = 0;
      // case 13:
      //   Serial.println("Repeat All");
      //   executeRepeats(2, 0);
      //   //executedQ[i] = commandsQ[i];
      //   commandsQ[i] = 0;
      default:
        Serial.println("Command not recognized");
        break;
    }
    delay(1000);
  }
}

void playEndTune(){
  tone(buzzerPin, 1000, 200);
  delay(200);
  tone(buzzerPin, 3000, 200);
  delay(200);
  tone(buzzerPin, 6000, 200);
  delay(200);
  //noTone(buzzerPin);
}

void readUltrasonic() {
  long duration;
  int distance;
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
    stopMove();
  }
}