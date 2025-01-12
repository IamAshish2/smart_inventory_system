// // #include <SPI.h>
// // #include <MFRC522.h>
// // #include <ESP32Servo.h>

// // // RFID
// // #define MFRC522_SPICLASS SPI
// // #define SS_PIN 5
// // #define RST_PIN 27

// // // PIR
// // #define PIR_PIN 25

// // MFRC522 mfrc522(SS_PIN, RST_PIN);
// // Servo servo;
// // // inventory door
// // int inventoryDoor = 22;
// // bool motionDetected = false;

// // // for the front door
// // int frontDoor = 21;


// // void setup() {
// //   Serial.begin(115200);
// //   SPI.begin();
// //   mfrc522.PCD_Init();
// //   Serial.println("System Initialized...");

// //   pinMode(PIR_PIN, INPUT);
// //   servo.attach(inventoryDoor);

// //   servo.write(180);  // Door closed initially
// // }

// // void loop() {
// //   // PIR Sensor
// //   int val = digitalRead(PIR_PIN);
// //   // Serial.println(val);
// //   if (val == HIGH) {
// //     // Serial.println("Motion detected!");
// //     handleMotion();
// //   }

// //   // RFID
// //   if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
// //     handleRfidActions();
// //   }
// // }

// // void handleMotion() {
// //   Serial.println("Handling motion...");
// //   servo.write(0);
// //   delay(1000); 
// //   // Simulate motion handling delay
// //   servo.write(90);
// // }

// // void handleRfidActions() {
// //   Serial.println("Reading RFID...");
// //   String content = "";

// //   for (byte i = 0; i < mfrc522.uid.size; i++) {
// //     content += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
// //     content += String(mfrc522.uid.uidByte[i], HEX);
// //   }
// //   content.toUpperCase();
// //   Serial.println("Card UID: " + content);

// //   if (content.substring(1) == "73 1F 27 2A") {
// //     Serial.println("Access Granted");
// //     servo.write(0);
// //     delay(3000);
// //     servo.write(180);
// //   } else {
// //     Serial.println("Access Not Granted");
// //   }

// //   mfrc522.PICC_HaltA();
// //   mfrc522.PCD_StopCrypto1();
// // }

#include <ESP32Servo.h>
#include <SPI.h>
#include <MFRC522.h>

// Ensure compatibility for ESP32
#define MFRC522_SPICLASS SPI

// Define pins for the RFID module
#define SS_PIN 5    // GPIO 5 for SS/SDA pin
#define RST_PIN 27  // GPIO 27 for RST pin

// Define pins for PIR sensor and servos
#define PIR_PIN 13      // GPIO connected to PIR sensor output
#define PIR_SERVO_PIN 21 // GPIO connected to PIR door servo
#define RFID_SERVO_PIN 22 // GPIO connected to Inventory door servo

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

Servo pirDoorServo;       // Servo for PIR motion detection
Servo inventoryDoorServo; // Servo for RFID-controlled inventory door

bool motionDetected = false;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize RFID module
  SPI.begin();           // Initialize SPI bus
  mfrc522.PCD_Init();    // Initialize RFID module
  Serial.println("Place an RFID card near the reader...");

  // Initialize PIR sensor pin
  pinMode(PIR_PIN, INPUT);

  // Attach servos to their respective pins
  pirDoorServo.attach(PIR_SERVO_PIN, 500, 2400);
  inventoryDoorServo.attach(RFID_SERVO_PIN, 500, 2400);

  // Set servos to their initial positions
  pirDoorServo.write(0);          // PIR door closed
  inventoryDoorServo.write(0);    // Inventory door closed

  Serial.println("System ready!");
}

// Servo positions for PIR door
int pirDoorClosed = 0;
int pirDoorOpen = 90;

// Servo positions for inventory door
int inventoryDoorClosed = 0;
int inventoryDoorOpen = 90;

void handleMotion() {
    // Read PIR sensor state
  motionDetected = digitalRead(PIR_PIN);

  if (motionDetected) {
    Serial.println("Motion detected!");
    pirDoorServo.write(pirDoorOpen);   // Move servo to 90 degrees (door open)
    delay(5000);             // Keep door open for 2 seconds
    pirDoorServo.write(pirDoorClosed); // Move servo back to 0 degrees (door closed)
    delay(1000);             // Small delay to prevent immediate retriggering
  }else{
    Serial.println("No motion detected");
  }

  delay(100); // Poll the PIR sensor every 100ms
}

void handleRFID() {
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

    inventoryDoorServo.write(inventoryDoorOpen);   // Open inventory door
    delay(2000);                                   // Keep door open for 2 seconds
    inventoryDoorServo.write(inventoryDoorClosed); // Close inventory door
    delay(1000);
  } else {
    Serial.println("Access Not Granted");
    Serial.println("Unauthorized");
  }

  // Halt PICC
  mfrc522.PICC_HaltA();

  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}

void loop() {
  handleMotion();
  handleRFID();
}
