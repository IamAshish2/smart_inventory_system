#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <ESP32Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include "HX711.h"
#include <time.h>

// Define WiFi credentials and Firebase configuration
#define WiFi_SSID "Netslowxa"
#define WiFi_PASSWORD "ubuntumaniac@67"
#define API_KEY "AIzaSyDbMyJGUj64t760G4ADqNR3Cr2C4gJslu0"
#define DATABASE_URL "https://smartdepartment-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Ensure compatibility for ESP32
#define MFRC522_SPICLASS SPI

// Define pins for the RFID module
#define SS_PIN 5    // GPIO 5 for SS/SDA pin
#define RST_PIN 27  // GPIO 27 for RST pin

// Define pins for PIR sensor and servos
#define PIR_PIN 15         // GPIO connected to PIR sensor output
#define PIR_SERVO_PIN 2    // GPIO connected to PIR door servo
#define RFID_SERVO_PIN 14  // GPIO connected to Inventory door servo 22

// Load cell pins and hx711 layout
const int INV_LOADCELL_DOUT_PIN = 13;
const int INV_LOADCELL_SCK_PIN = 12;
long weight;
HX711 scale;

const int STORE_LOADCELL_DOUT_PIN_1 = 26;
const int STORE_LOADCELL_SCK_PIN_1 = 25;
long storeWeight1;
HX711 STORESCALE1;

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

Servo pirDoorServo;        // Servo for PIR motion detection
Servo inventoryDoorServo;  // Servo for RFID-controlled inventory door

bool motionDetected = false;

// Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


unsigned long sendDataPrevMillis = 0;
bool signUpOk = false;

// Servo positions for PIR door
int pirDoorClosed = 0;
int pirDoorOpen = 180;

// Servo positions for inventory door
int inventoryDoorClosed = 0;
int inventoryDoorOpen = 180;

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WiFi_SSID);
  WiFi.begin(WiFi_SSID, WiFi_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected to WiFi");
  Serial.println("IP Address: " + WiFi.localIP().toString());
}

void setupFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase signup successful");
    signUpOk = true;
  } else {
    Serial.printf("Signup failed: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

// void handleMotion() {
//   // Read PIR sensor state
//   motionDetected = digitalRead(PIR_PIN);

//   if (motionDetected) {
//     Serial.println("Motion detected!");
//     pirDoorServo.write(pirDoorOpen);  // Move servo to 90 degrees (door open)
//     Serial.println("Door opening");
//     delay(3000);                        // Keep door open for 2 seconds
//     pirDoorServo.write(pirDoorClosed);  // Move servo back to 0 degrees (door closed)
//     delay(1000);                        // Small delay to prevent immediate retriggering
//   } else {
//     // Serial.println("no motion");
//     return;
//   }

//   delay(100);  // Poll the PIR sensor every 100ms
// }

void handleMotion() {
  // Read PIR sensor state
  motionDetected = digitalRead(PIR_PIN);

  if (motionDetected) {
    Serial.println("Motion detected!");

    // Get the current time for door opening
    String openTime = getFormattedTime();
    if (Firebase.RTDB.setString(&fbdo, "Motion/door/opened_at", openTime)) {
      Serial.printf("Door opened at: %s saved successfully to Firebase\n", openTime.c_str());
    } else {
      Serial.printf("Failed to send open time: %s\n", fbdo.errorReason().c_str());
    }

    pirDoorServo.write(pirDoorOpen);  // Move servo to 90 degrees (door open)
    Serial.println("Door opening");
    delay(3000);  // Keep door open for 3 seconds

    // Get the current time for door closing
    String closeTime = getFormattedTime();
    if (Firebase.RTDB.setString(&fbdo, "Motion/door/closed_at", closeTime)) {
      Serial.printf("Door closed at: %s saved successfully to Firebase\n", closeTime.c_str());
    } else {
      Serial.printf("Failed to send close time: %s\n", fbdo.errorReason().c_str());
    }

    pirDoorServo.write(pirDoorClosed);  // Move servo back to 0 degrees (door closed)
    Serial.println("Door closing");
    delay(1000);  // Small delay to prevent immediate retriggering
  } else {
    // No motion detected
    return;
  }

  delay(100);  // Poll the PIR sensor every 100ms
}

// Function to get the current formatted time
String getFormattedTime() {
  time_t now = time(nullptr);  // Get current time
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
  return String(buffer);
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

  if (content.substring(1) == "73 1F 27 2A" || content.substring(1) == "43 71 1B 0D") {
    Serial.println("Access Granted");
        // Keep door open for 2 seconds
    inventoryDoorServo.write(inventoryDoorOpen);  // Open inventory door
    delay(2000);

    if (Firebase.ready() && signUpOk && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
      sendDataPrevMillis = millis();

      // Send door open time to Firebase
      String openTime = getFormattedTime();
      if (Firebase.RTDB.setString(&fbdo, "Inventory/door/opened_at", openTime)) {
        Serial.printf("Door opened at: %s saved successfully to Firebase\n", openTime.c_str());
      } else {
        Serial.printf("Failed to send open time: %s\n", fbdo.errorReason().c_str());
      }
    }

    // Send door close time to Firebase
    String closeTime = getFormattedTime();
    if (Firebase.RTDB.setString(&fbdo, "Inventory/door/closed_at", closeTime)) {
      Serial.printf("Door closed at: %s saved successfully to Firebase\n", closeTime.c_str());
    } else {
      Serial.printf("Failed to send close time: %s\n", fbdo.errorReason().c_str());
    }

    inventoryDoorServo.write(inventoryDoorClosed);  // Close inventory door
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

void setup() {
  Serial.begin(115200);

  // setupTime();

  // Connect to WiFi
  connectWiFi();

  // Initialize Firebase
  setupFirebase();
  // Initialize RFID module
  SPI.begin();         // Initialize SPI bus
  mfrc522.PCD_Init();  // Initialize RFID module
  Serial.println("Place an RFID card near the reader...");

  // Initialize PIR sensor pin
  // pinMode(PIR_PIN, INPUT);
  pinMode(PIR_PIN, INPUT_PULLUP);

  // Attach servos to their respective pins
  pirDoorServo.attach(PIR_SERVO_PIN, 500, 2400);
  inventoryDoorServo.attach(RFID_SERVO_PIN, 500, 2400);

  // Set servos to their initial positions
  pirDoorServo.write(0);        // PIR door closed
  inventoryDoorServo.write(0);  // Inventory door closed

  // Initialize HX711
  Serial.println("Initializing INVENTORY load cell...");
  scale.begin(INV_LOADCELL_DOUT_PIN, INV_LOADCELL_SCK_PIN);
  scale.set_scale(-427);  // Adjust as per your calibration
  scale.tare();
  Serial.println("Load cell initialized.");

  // Initialize store load cell
  Serial.println("Initializing store load cell...");
  STORESCALE1.begin(STORE_LOADCELL_DOUT_PIN_1, STORE_LOADCELL_SCK_PIN_1);
  STORESCALE1.set_scale(-223);
  STORESCALE1.tare();

  Serial.println("System ready!");
}

void calculateInventoryWeight() {
  weight = scale.get_units(10);
  Serial.print("inventory weight");
  Serial.println(weight);

  scale.power_down();  // Power down ADC to save power
  delay(1000);
  scale.power_up();

  if (Firebase.ready() && signUpOk && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    // Send data to Firebase
    if (Firebase.RTDB.setInt(&fbdo, "Inventory/stock", weight)) {
      Serial.printf("Weight: %ld saved successfully to %s (%s)\n", weight, fbdo.dataPath().c_str(), fbdo.dataType().c_str());
    } else {
      Serial.printf("Data send failed: %s\n", fbdo.errorReason().c_str());
    }
  }
}

void calculateStoreWeight1() {
  storeWeight1 = STORESCALE1.get_units(10);
  Serial.print("Load cell weight ");
  Serial.println(storeWeight1);
  scale.power_down();  // set ADC to sleep mode
  delay(1000);
  scale.power_up();

  if (Firebase.ready() && signUpOk && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    // Send data to Firebase
    if (Firebase.RTDB.setInt(&fbdo, "Store/products", storeWeight1)) {
      Serial.printf("Weight: %ld saved successfully to %s (%s)\n", storeWeight1, fbdo.dataPath().c_str(), fbdo.dataType().c_str());
    } else {
      Serial.printf("Data send failed: %s\n", fbdo.errorReason().c_str());
    }
  }
}

void loop() {
  handleMotion();
  handleRFID();
  calculateInventoryWeight();
  calculateStoreWeight1();
}
