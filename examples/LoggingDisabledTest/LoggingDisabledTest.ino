/*
  Logging Disabled Test - Verify that NDP logging is disabled by default
  This should execute setup() and loop() without any NDP library logs
*/

#include "Arduino.h"
#include "NDP.h"

uint8_t data[2048];
int chunk_size = 0;
unsigned long total_bytes_captured = 0;
unsigned long last_report_time = 0;
bool setup_complete = false;

void ledGreenOn() {
  nicla::leds.begin();
  nicla::leds.setColor(green);
  delay(200);
  nicla::leds.setColor(off);
  nicla::leds.end();
}

void setup() {
  Serial.begin(1000000); // 1 Mbps
  while (!Serial) delay(10);

  Serial.println("=== LOGGING DISABLED TEST START ===");
  Serial.println("Arduino Nicla Voice - Logging Disabled Test");
  Serial.println("Timestamp: " + String(millis()));

  // Test default logging state (should be disabled)
  Serial.println("NDP Logging (Default): " + String(NDP.isLoggingEnabled() ? "Enabled" : "Disabled"));

  nicla::begin();
  nicla::disableLDO();
  nicla::leds.begin();

  NDP.onEvent(ledGreenOn);

  Serial.println("MCU:LOAD_START (with logging DISABLED by default)");
  NDP.begin("mcu_fw_120_v91.synpkg"); // This should NOT log anything
  Serial.println("MCU:LOAD_DONE");

  Serial.println("DSP:LOAD_START (with logging DISABLED)");
  NDP.load("dsp_firmware_v91.synpkg"); // This should NOT log anything
  Serial.println("DSP:LOAD_DONE");

  Serial.println("NN:LOAD_START (with logging DISABLED)");
  NDP.load("alexa_334_NDP120_B0_v11_v91.synpkg"); // This should NOT log anything
  Serial.println("NN:LOAD_DONE");

  NDP.turnOnMicrophone();
  chunk_size = NDP.getAudioChunkSize();
  Serial.println("CHUNK:SIZE=" + String(chunk_size));

  if (chunk_size >= sizeof(data)) {
    Serial.println("ERROR:CHUNK_SIZE_TOO_LARGE");
    for(;;);
  }

  Serial.println("SETUP:COMPLETE");
  Serial.println("=== STARTING AUDIO CAPTURE LOOP ===");
  last_report_time = millis();
  setup_complete = true;
}

void loop() {
  if (!setup_complete) return;

  unsigned int len = 0;
  NDP.extractData(data, &len);

  if (len > 0) {
    total_bytes_captured += len;
  }

  if (millis() - last_report_time >= 1000) { // Report every 1 second
    Serial.println("STATS:BYTES_CAPTURED=" + String(total_bytes_captured));
    last_report_time = millis();
  }
  delay(10);
}
