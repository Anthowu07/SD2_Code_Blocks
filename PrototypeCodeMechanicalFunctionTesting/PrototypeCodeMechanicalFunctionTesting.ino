#include <MFRC522.h>
#include <SPI.h>

/*  
  Three states determined from variable status.



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

byte readCard[4];
char* myTags[100] = {};
int tagsCount = 0;
String tagID = "";
boolean successRead = false;
boolean correctTag = false;
int status = 0; // Controls the status of the motors
long duration;
int distance;

// Create instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

int rotDirection = 0;

void setup() {

  // Initiating
  SPI.begin();        // SPI bus
  mfrc522.PCD_Init(); //  MFRC522
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
  
}

uint8_t getID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  tagID = "";
  for ( uint8_t i = 0; i < 4; i++) {  // The MIFARE PICCs that we use have 4 byte UID
    readCard[i] = mfrc522.uid.uidByte[i];
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
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
void forwardMove(int speed) {
  analogWrite(enA, speed); // Send PWM signal to L298N Enable pin
  analogWrite(enB, speed); // Send PWM signal to L298N Enable pin
  analogWrite(enC, speed); // Send PWM signal to L298N Enable pin
  analogWrite(enD, speed); // Send PWM signal to L298N Enable pin

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
void backwardMove(int speed) {

  // Inverse in pin values to change to backwards movement
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  digitalWrite(in5, LOW);
  digitalWrite(in6, HIGH);
  digitalWrite(in7, LOW);
  digitalWrite(in8, HIGH); 
  
  analogWrite(enA, speed); // Send PWM signal to L298N Enable pin
  analogWrite(enB, speed); // Send PWM signal to L298N Enable pin
  analogWrite(enC, speed); // Send PWM signal to L298N Enable pin
  analogWrite(enD, speed); // Send PWM signal to L298N Enable pin

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

void diagonalForwardRight() {

}

void diagonalForwardLeft() {

}

void stopMove() {
  analogWrite(enA, 0); // Send turn off signal
  analogWrite(enB, 0); // Send turn off signal
  analogWrite(enC, 0); // Send turn off signal
  analogWrite(enD, 0); // Send turn off signal
} 

int pressed = 0;
int toggle = 0;
int speed = 255;
void loop() {

    // Read button - Debounce
  if (digitalRead(button) == true) {
    pressed = !pressed;
  }
  while (digitalRead(button) == true);
  delay(20);

  if (pressed == true) {
    forwardMove(speed);
    sidewayRight();
    sidewayLeft();
    rotateLeft();
    rotateRight();
    backwardMove(speed);
    pressed = 0;
  }
  /*else if (pressed == false) {
    stopMove();
  }*/



    // Read button - Debounce
  if (digitalRead(button2) == true) {
    toggle = !toggle;
  }
  while (digitalRead(button2) == true);
  delay(20);

  if (toggle == true) {
    rotateRight();
    rotateRight();
    rotateRight();
    rotateRight();
    toggle = 0;
  }
  /*
  // If button is pressed - change rotation direction
  if (pressed == true  & rotDirection == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    rotDirection = 1;
    Serial.println(rotDirection);
    delay(20);
  }

  // If button is pressed - change rotation direction
  if (pressed == false & rotDirection == 1) {
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    rotDirection = 0;
    Serial.println(rotDirection);
    delay(20);
  }
  */


  
  //readUltrasonic();

  // If status is set to 2 (Porward) and not already in that direction
  /*
  if (status == 2  & rotDirection == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    rotDirection = 1;
  }
  // If status is set to 3 (Backwards) and not already in that direction
  if (status == 3 & rotDirection == 1) {
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    rotDirection = 0;
  }
  */
}
