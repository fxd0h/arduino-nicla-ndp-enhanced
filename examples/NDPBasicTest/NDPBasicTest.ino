#include "Arduino.h"
#include "NDP.h"

void setup() {
  Serial.begin(1000000);  // 1 Mbps
  while (!Serial) delay(10);
  
  Serial.println("=== NDP BASIC TEST START ===");
  Serial.println("Arduino Nicla Voice - NDP Basic Test");
  Serial.println("Timestamp: " + String(millis()));
  
  // Initialize Nicla board components
  nicla::begin();
  nicla::disableLDO();
  nicla::leds.begin();

  Serial.println("GPIO: OK");

  // Test NDP.begin() with minimal logging
  Serial.println("NDP:INIT_START");
  Serial.println("MCU:BEFORE_NDP_BEGIN");
  
  // Try to initialize NDP with minimal firmware
  int ret = NDP.begin("mcu_fw_120_v91.synpkg");
  
  Serial.println("MCU:AFTER_NDP_BEGIN");
  Serial.print("NDP:INIT_RESULT=");
  Serial.println(ret);
  
  if (ret != 0) {
    Serial.print("NDP:INIT_FAIL: ");
    Serial.println(ret);
    Serial.println("NDP:CONTINUING_ANYWAY");
  } else {
    Serial.println("NDP:INIT_OK");
  }

  Serial.println("=== NDP BASIC TEST SETUP COMPLETE ===");
}

void loop() {
  Serial.println("LOOP: " + String(millis()));
  delay(1000);
}
