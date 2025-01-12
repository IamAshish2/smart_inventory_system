/*
SDA - D15
SCK - D13
MOSI - D12
MISO - D14
RST - D25

// #include <SPI.h>
// #include <MFRC522.h>
// #include <ESP32Servo.h>

// // Pin definitions
// #define SS_PIN    5
// #define RST_PIN   27
// #define SERVO_PIN 13

// // Create instances
// MFRC522 rfid(SS_PIN, RST_PIN);
// Servo myServo;

// // Store authorized card UID
// byte authorizedUID[4] = {0x00, 0x00, 0x00, 0x00};  // Replace with your card's UID

// void setup() {
//   // Initialize serial communication
//   Serial.begin(115200);

//   // Initialize SPI bus
//   SPI.begin();

//   // Initialize RFID reader
//   rfid.PCD_Init();
//   Serial.println("RFID reader initialized");

//   // Initialize servo
//   ESP32PWM::allocateTimer(0);
//   myServo.setPeriodHertz(50);  // Standard 50Hz servo
//   myServo.attach(SERVO_PIN, 500, 2400);  // Adjust min/max pulse width if needed

//   // Move servo to initial position
//   myServo.write(0);
// }

// void loop() {
//   // Check if a new card is present
//   if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
//     return;

//   Serial.print("Card UID: ");
//   for (byte i = 0; i < rfid.uid.size; i++) {
//     Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
//     Serial.print(rfid.uid.uidByte[i], HEX);
//   }
//   Serial.println();

//   // Check if it's the authorized card
//   if (compareUID(rfid.uid.uidByte, authorizedUID)) {
//     Serial.println("Access granted!");
//     // Open door (rotate servo to 90 degrees)
//     myServo.write(90);
//     delay(3000);  // Keep door open for 3 seconds
//     // Close door (return to 0 degrees)
//     myServo.write(0);
//   } else {
//     Serial.println("Access denied!");
//   }

//   // Halt PICC and stop encryption
//   rfid.PICC_HaltA();
//   rfid.PCD_StopCrypto1();
// }

// // Helper function to compare UIDs
// bool compareUID(byte* uid1, byte* uid2) {
//   for (byte i = 0; i < 4; i++) {
//     if (uid1[i] != uid2[i]) return false;
//   }
//   return true;
// }
/* Read RFID Tag with RC522 RFID Reader
 *  Made by miliohm.com
 */
/* Read RFID Tag with RC522 RFID Reader
 *  Made by miliohm.com
 */
/* Read RFID Tag with RC522 RFID Reader
 *  Modified for ESP32
 */
/* Read RFID Tag with RC522 RFID Reader
 *  Made by miliohm.com
 */




// 73 1F 27 2A

#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>


// Ensure compatibility for ESP32
#define MFRC522_SPICLASS SPI

// Define pins for the RFID module
#define SS_PIN 5    // GPIO 5 for SS/SDA pin
#define RST_PIN 27  // GPIO 27 for RST pin

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

int doorPin = 22;

Servo servo;

void setup() {
  Serial.begin(115200);  // Initialize serial communication
  SPI.begin();           // Initialize SPI bus
  mfrc522.PCD_Init();    // Initialize RFID module
  Serial.println("Place an RFID card near the reader...");

  servo.attach(doorPin);
  servo.write(0);
}

void loop() {
  // Check if a new card is present
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;  // Exit if no card is present
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;  // Exit if card could not be read
  }

  String content = "";

  // Print card UID
  Serial.print("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");  // Add leading zero for single hex digits
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));  // Print UID in hexadecimal format
    content.concat(String(mfrc522.uid.uidByte[i], HEX));                 // Print UID in hexadecimal format
  }
  Serial.println();

  content.toUpperCase();

  if (content.substring(1) == "73 1F 27 2A") {
    Serial.println("Access Granted");

    servo.write(300);
    Serial.println("door closing");
    delay(1000);
    servo.write(0);
    // Serial.println("door closed");
  } else{
    Serial.println("Access Not Granted");
    Serial.println("Unauthorized");
  }

  // Halt PICC
  mfrc522.PICC_HaltA();

  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}
