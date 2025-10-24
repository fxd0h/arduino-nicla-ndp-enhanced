#include "Arduino.h"
#include "Nicla_System.h"

void setup() {
  Serial.begin(1000000);  // 1 Mbps
  while (!Serial) delay(10);
  
  Serial.println("=== SETUP DEBUG TEST START ===");
  Serial.println("Arduino Nicla Voice - Setup Debug Test");
  Serial.println("Timestamp: " + String(millis()));
  
  Serial.println("STEP1: Serial initialized");
  
  // Initialize Nicla board components
  Serial.println("STEP2: Before nicla::begin()");
  nicla::begin();
  Serial.println("STEP3: After nicla::begin()");
  
  Serial.println("STEP4: Before nicla::disableLDO()");
  nicla::disableLDO();
  Serial.println("STEP5: After nicla::disableLDO()");
  
  Serial.println("STEP6: Before nicla::leds.begin()");
  nicla::leds.begin();
  Serial.println("STEP7: After nicla::leds.begin()");

  Serial.println("GPIO: OK");

  Serial.println("=== SETUP DEBUG TEST SETUP COMPLETE ===");
}

void loop() {
  Serial.println("LOOP: " + String(millis()));
  delay(1000);
}
