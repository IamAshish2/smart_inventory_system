#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include "HX711.h"

// Define WiFi credentials and Firebase configuration
#define WiFi_SSID "Netslowxa"
#define WiFi_PASSWORD "ubuntumaniac@67"
#define API_KEY "AIzaSyDbMyJGUj64t760G4ADqNR3Cr2C4gJslu0"
#define DATABASE_URL "https://smartdepartment-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Load cell pins
const int LOADCELL_DOUT_PIN = 13;
const int LOADCELL_SCK_PIN = 12;

// Firebase and HX711 objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
HX711 scale;

unsigned long sendDataPrevMillis = 0;
bool signUpOk = false;
long weight;

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

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  connectWiFi();

  // Initialize Firebase
  setupFirebase();

  // Initialize HX711
  Serial.println("Initializing load cell...");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-451.75); // Adjust as per your calibration
  scale.tare();
  Serial.println("Load cell initialized.");
}

void loop() {
  weight = scale.get_units(10);

  scale.power_down(); // Power down ADC to save power
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
 