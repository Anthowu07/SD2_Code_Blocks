#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

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
  motorSetup();

  for (int i = 0; i < READERS; i++) {
    nfcDevices[i].begin();
  }

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
