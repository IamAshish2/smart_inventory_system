// /*
//  * This ESP32 code is created by esp32io.com
//  *
//  * This ESP32 code is released in the public domain
//  *
//  * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-rfid-nfc-door-lock-system
//  */

// // RFID 

// #include <SPI.h>
// #include <MFRC522.h>

// #define SS_PIN    5  // ESP32 pin GPIO5 
// #define RST_PIN   27 // ESP32 pin GPIO27 
// #define RELAY_PIN 32 // ESP32 pin GPIO32 connects to relay

// MFRC522 rfid(SS_PIN, RST_PIN);

// byte keyTagUID[4] = {0xFF, 0xFF, 0xFF, 0xFF};

// void setup() {
//   Serial.begin(9600);
//   SPI.begin(); // init SPI bus
//   rfid.PCD_Init(); // init MFRC522
//   pinMode(RELAY_PIN, OUTPUT); // initialize pin as an output.
//   digitalWrite(RELAY_PIN, HIGH); // lock the door

//   Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader");
// }

// void loop() {
//   if (rfid.PICC_IsNewCardPresent()) { // new tag is available
//     if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
//       MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

//       if (rfid.uid.uidByte[0] == keyTagUID[0] &&
//           rfid.uid.uidByte[1] == keyTagUID[1] &&
//           rfid.uid.uidByte[2] == keyTagUID[2] &&
//           rfid.uid.uidByte[3] == keyTagUID[3] ) {
//         Serial.println("Access is granted");
//         digitalWrite(RELAY_PIN, LOW);  // unlock the door for 2 seconds
//         delay(2000);
//         digitalWrite(RELAY_PIN, HIGH); // lock the door
//       }
//       else
//       {
//         Serial.print("Access denied, UID:");
//         for (int i = 0; i < rfid.uid.size; i++) {
//           Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
//           Serial.print(rfid.uid.uidByte[i], HEX);
//         }
//         Serial.println();
//       }

//       rfid.PICC_HaltA(); // halt PICC
//       rfid.PCD_StopCrypto1(); // stop encryption on PCD
//     }
//   }
// }




#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN    5 //10
#define RST_PIN   12 // 9
#define SERVO_PIN A5

MFRC522 rfid(SS_PIN, RST_PIN);
Servo servo;

byte authorizedUID[4] = {0xF7, 0x3F, 0x16, 0x62};
int angle = 0; // the current angle of servo motor

void setup() {
  Serial.begin(9600);
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  servo.attach(SERVO_PIN);
  servo.write(angle); // rotate servo motor to 0°

  Serial.println("Tap RFID/NFC Tag on reader");
}

void loop() {
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

      if (rfid.uid.uidByte[0] == authorizedUID[0] &&
          rfid.uid.uidByte[1] == authorizedUID[1] &&
          rfid.uid.uidByte[2] == authorizedUID[2] &&
          rfid.uid.uidByte[3] == authorizedUID[3] ) {
        Serial.println("Authorized Tag");

        // change angle of servo motor
        if (angle == 0)
          angle = 90;
        else //if(angle == 90)
          angle = 0;

        // control servo motor arccoding to the angle
        servo.write(angle);
        Serial.print("Rotate Servo Motor to ");
        Serial.print(angle);
        Serial.println("°");
      } else {
        Serial.print("Unauthorized Tag with UID:");
        for (int i = 0; i < rfid.uid.size; i++) {
          Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(rfid.uid.uidByte[i], HEX);
        }
        Serial.println();
      }

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
}