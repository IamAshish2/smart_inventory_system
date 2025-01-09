#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>

// RFID
#define MFRC522_SPICLASS SPI
#define SS_PIN 5
#define RST_PIN 27

// PIR
#define PIR_PIN 22

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo servo;

int doorPin = 22;
bool motionDetected = false;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("System Initialized...");

  pinMode(PIR_PIN, INPUT);
  servo.attach(doorPin);
  servo.write(180);  // Door closed initially
}

void loop() {
  // PIR Sensor
  int val = digitalRead(PIR_PIN);
  // Serial.println(val);
  if (val == HIGH) {
    // Serial.println("Motion detected!");
    handleMotion();
  }

  // RFID
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    handleRfidActions();
  }
}

void handleMotion() {
  Serial.println("Handling motion...");
  servo.write(180);
  delay(1000); // Simulate motion handling delay
}

void handleRfidActions() {
  Serial.println("Reading RFID...");
  String content = "";

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    content += String(mfrc522.uid.uidByte[i], HEX);
  }
  content.toUpperCase();
  Serial.println("Card UID: " + content);

  if (content.substring(1) == "73 1F 27 2A") {
    Serial.println("Access Granted");
    servo.write(0);
    delay(3000);
    servo.write(180);
  } else {
    Serial.println("Access Not Granted");
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
