// /*
//  * This ESP32 code is created by esp32io.com
//  *
//  * This ESP32 code is released in the public domain
//  *
//  * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-motion-sensor
//  */

// const int PIN_TO_SENSOR = 19; // GPIO19 pin connected to OUTPUT pin of sensor
// int pinStateCurrent   = LOW;  // current state of pin
// int pinStatePrevious  = LOW;  // previous state of pin

// void setup() {
//   Serial.begin(9600);            // initialize serial
//   pinMode(PIN_TO_SENSOR, INPUT); // set ESP32 pin to input mode to read value from OUTPUT pin of sensor
// }

// void loop() {
//   pinStatePrevious = pinStateCurrent; // store old state
//   pinStateCurrent = digitalRead(PIN_TO_SENSOR);   // read new state

//   if (pinStatePrevious == LOW && pinStateCurrent == HIGH) {   // pin state change: LOW -> HIGH
//     Serial.println("Motion detected!");
//     // TODO: turn on alarm, light or activate a device ... here
//   }
//   else
//   if (pinStatePrevious == HIGH && pinStateCurrent == LOW) {   // pin state change: HIGH -> LOW
//     Serial.println("Motion stopped!");
//     // TODO: turn off alarm, light or deactivate a device ... here
//   }
//   // delay(1000);
// }

// check the PIR sensor
// #define PIR_PIN 13

// void setup() {
//   Serial.begin(115200);
//   pinMode(PIR_PIN, INPUT);
//   delay(30000);
//   Serial.println("Testing PIR Sensor...");
// }

// void loop() {
//   int motionDetected = digitalRead(PIR_PIN);

//   if (motionDetected) {
//     Serial.println("Motion detected!");
//   } else {
//     Serial.println("No motion detected.");
//   }

//   delay(500);
// }



#include <ESP32Servo.h>

// Define pins
#define PIR_PIN 13      // GPIO connected to PIR sensor output
#define SERVO_PIN 21     // GPIO connected to Servo motor signal

Servo servo;
bool motionDetected = false;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize PIR sensor pin
  pinMode(PIR_PIN, INPUT);

  // Attach the servo to the defined pin
  servo.attach(SERVO_PIN, 500, 2400);  // Attach with min/max pulse widths (500-2400µs)

  // Set servo to the initial position
  servo.write(0);  // Initial angle
  delay(30000);

  Serial.println("System ready!");
}

// Servo positions
int initialPos = 0;     // Servo angle when door is closed
int doorOpen = 90;      // Servo angle when door is open

void loop() {
  // Read PIR sensor state
  motionDetected = digitalRead(PIR_PIN);

  if (motionDetected) {
    Serial.println("Motion detected!");
    servo.write(doorOpen);   // Move servo to 90 degrees (door open)
    delay(2000);             // Keep door open for 2 seconds
    servo.write(initialPos); // Move servo back to 0 degrees (door closed)
    delay(1000);             // Small delay to prevent immediate retriggering
  }else{
    Serial.println("No motion detected");
  }

  delay(100); // Poll the PIR sensor every 100ms
}

