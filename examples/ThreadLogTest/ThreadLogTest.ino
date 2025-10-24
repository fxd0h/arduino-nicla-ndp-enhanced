#include "Arduino.h"
#include "NDP.h"

void setup() {
  Serial.begin(1000000);  // 1 Mbps
  while (!Serial) delay(10);
  
  Serial.println("=== THREAD LOG TEST START ===");
  Serial.println("Arduino Nicla Voice - Thread Log Test");
  Serial.println("Timestamp: " + String(millis()));
  
  // Initialize Nicla board components
  nicla::begin();
  nicla::disableLDO();
  nicla::leds.begin();

  // Configure NDP120 pins
  pinMode(31, OUTPUT);  // CS
  pinMode(18, OUTPUT);  // RST
  pinMode(30, INPUT);   // INT

  digitalWrite(31, HIGH); // Ensure CS is high initially
  digitalWrite(18, HIGH); // Ensure RST is high initially

  Serial.println("GPIO: OK");

  // Reset NDP120
  Serial.println("NDP:RESET_START");
  digitalWrite(18, LOW);
  delay(100);
  digitalWrite(18, HIGH);
  delay(100);
  Serial.println("NDP:RESET_END");

  // Test NDP.begin() with async logging
  Serial.println("NDP:INIT_START");
  Serial.println("MCU:BEFORE_NDP_BEGIN");
  int ret = NDP.begin("mcu_fw_120_v91.synpkg");
  Serial.println("MCU:AFTER_NDP_BEGIN");
  if (ret != 0) {
    Serial.print("NDP:INIT_FAIL: ");
    Serial.println(ret);
    while(1); // Halt on error
  }
  Serial.println("NDP:INIT_OK");

  Serial.println("=== THREAD LOG TEST SETUP COMPLETE ===");
}

void loop() {
  Serial.println("LOOP: " + String(millis()));
  delay(1000);
}
