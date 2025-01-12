// #include "HX711.h"

// // HX711 circuit wiring
// const int LOADCELL_DOUT_PIN = 22;
// const int LOADCELL_SCK_PIN = 23;

// HX711 scale;

// //DIY_CHEAP_PERFECT

// void setup() {
//   Serial.begin(115200);
//   scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
//   scale.tare();                // reset the scale to 0
// }


// void loop() {

//   if (scale.is_ready()) {
//     Serial.print("HX711 reading: ");
//     Serial.println(scale.get_value(5)); // print the average of 5 readings from the ADC minus the tare weight, set with tare()
//   } 
//   else if(!scale.is_ready()) {
//     Serial.println("HX711 not found.");
//   }

//   delay(500);
  
// }


// #include "HX711.h"

// // HX711 circuit wiring
// const int LOADCELL_DOUT_PIN = 2;
// const int LOADCELL_SCK_PIN = 4;

// HX711 scale;

// void setup() {
//   Serial.begin(230400);
//   scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
//   Serial.println("Remove any weights from the scale and press any key to tare...");
  
//   while (!Serial.available()); // Wait for user input
//   scale.tare();                // Reset the scale to 0
//   Serial.println("Scale tared. Place a known weight on the scale.");
// }

// void loop() {
//   if (scale.is_ready()) {
//     Serial.print("Raw ADC Value: ");
//     Serial.println(scale.get_value(10)); // Average of 10 readings
//     Serial.println("Enter the known weight in grams:");
    
//     if (Serial.available()) {
//       String input = Serial.readString();
//       float knownWeight = input.toFloat();

//       if (knownWeight > 0) {
//         float calibrationFactor = scale.get_value(10) / knownWeight;
//         Serial.print("Calibration Factor: ");
//         Serial.println(calibrationFactor);
//         Serial.println("Update your program with this calibration factor.");
//       } else {
//         Serial.println("Invalid weight. Try again.");
//       }
//       while (Serial.available()) Serial.read(); // Clear the input buffer
//     }
//   } else {
//     Serial.println("HX711 not found.");
//   }

//   delay(500);
// }






















#include "HX711.h"

// Define HX711 pins
#define DT 23   // Data pin
#define SCK 22  // Clock pin

HX711 scale;

void setup() {
  Serial.begin(115200);

  // Initialize the scale
  scale.begin(DT, SCK);

  // Calibrate the scale (replace with your calibration factor)
  Serial.println("Calibrating scale...");
  // scale.set_scale(2280.f); // Adjust this value to calibrate the scale
  scale.tare();            // Reset the scale to 0
  Serial.println("Scale initialized!");
}

void loop() {
  if (scale.is_ready()) {
    // Read the weight in grams
    float weight = scale.get_units();
    Serial.print("Weight: ");
    Serial.print(weight, 2); // Print weight with 2 decimal points
    Serial.println(" g");
  } else {
    Serial.println("Scale not ready");
  }

  delay(1000); // Wait 1 second between readings
}


