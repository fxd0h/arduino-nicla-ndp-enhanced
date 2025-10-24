#include "Arduino.h"

// Hardcoded pin definitions
#define NDP_CS_PIN 31
#define NDP_INT_PIN 30
#define NDP_RST_PIN 18

void setup() {
  Serial.begin(1000000);  // 1 Mbps
  while (!Serial) delay(10);
  
  Serial.println("=== NDP120 SPI TEST START ===");
  Serial.println("Arduino Nicla Voice - NDP120 SPI Test");
  Serial.println("Timestamp: " + String(millis()));
  
  // Configure NDP120 pins
  pinMode(NDP_CS_PIN, OUTPUT);
  pinMode(NDP_RST_PIN, OUTPUT);
  pinMode(NDP_INT_PIN, INPUT);

  digitalWrite(NDP_CS_PIN, HIGH); // Ensure CS is high initially
  digitalWrite(NDP_RST_PIN, HIGH); // Ensure RST is high initially

  Serial.println("GPIO: OK");

  // Reset NDP120
  Serial.println("NDP:RESET_START");
  digitalWrite(NDP_RST_PIN, LOW);
  delay(100);
  digitalWrite(NDP_RST_PIN, HIGH);
  delay(100);
  Serial.println("NDP:RESET_END");

  // Test basic SPI communication
  Serial.println("SPI:INIT_START");
  SPI1.begin();
  SPI1.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  Serial.println("SPI:INIT_OK");

  // Test Chip ID read (basic SPI test)
  Serial.println("CHIP:TEST_START");
  
  digitalWrite(NDP_CS_PIN, LOW);
  uint8_t chip_id0 = SPI1.transfer(0x80);  // Read ID0
  uint8_t chip_id1 = SPI1.transfer(0x00);  // Read ID1
  uint8_t chip_id2 = SPI1.transfer(0x00);  // Read ID2
  uint8_t chip_id3 = SPI1.transfer(0x00);  // Read ID3
  digitalWrite(NDP_CS_PIN, HIGH);
  
  Serial.print("CHIP_ID:0x");
  if (chip_id3 < 16) Serial.print("0");
  Serial.print(chip_id3, HEX);
  if (chip_id2 < 16) Serial.print("0");
  Serial.print(chip_id2, HEX);
  if (chip_id1 < 16) Serial.print("0");
  Serial.print(chip_id1, HEX);
  if (chip_id0 < 16) Serial.print("0");
  Serial.println(chip_id0, HEX);
  
  if (chip_id0 == 0x20 && chip_id1 == 0x01 && chip_id2 == 0x00 && chip_id3 == 0x00) {
    Serial.println("NDP:OK - NDP120 responding");
  } else {
    Serial.println("NDP:FAIL - NDP120 not responding");
  }
  
  Serial.println("=== NDP120 SPI TEST COMPLETE ===");
}

void loop() {
  Serial.println("LOOP: " + String(millis()));
  delay(1000);
}
