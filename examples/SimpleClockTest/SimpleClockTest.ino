/**
 * @file SimpleClockTest.ino
 * @brief Simple NDP120 Clock Configuration Test
 */

#include <NDP.h>

void setup() {
  Serial.begin(115200);
  delay(3000);  // Wait for serial monitor
  
  Serial.println("=== Simple Clock Test ===");
  
  // Initialize NDP120 with MCU firmware
  Serial.println("Initializing NDP120...");
  if (!NDP.begin("mcu_fw_120_v91.synpkg")) {
    Serial.println("ERROR: NDP120 init failed!");
    return;
  }
  Serial.println("✓ NDP120 initialized");
  
  // Load DSP firmware
  Serial.println("Loading DSP firmware...");
  if (!NDP.load("dsp_firmware_v91.synpkg")) {
    Serial.println("ERROR: DSP load failed!");
    return;
  }
  Serial.println("✓ DSP firmware loaded");
  
  // Test clock configuration
  Serial.println("Testing clock configuration...");
  
  // Get the internal ndp structure
  extern struct syntiant_ndp120_tiny_device_s *ndp;
  if (ndp == NULL) {
    Serial.println("ERROR: ndp structure not accessible!");
    return;
  }
  
  Serial.print("Before clock_cfg - pkg_load_flag: 0x");
  Serial.println(ndp->pkg_load_flag, HEX);
  
  // Configure clock with FLL settings
  struct syntiant_ndp120_tiny_clk_config_data clk_cfg;
  clk_cfg.src = 3;         // FLL source
  clk_cfg.ref_freq = 32768; // 32.768 kHz reference
  clk_cfg.ref_type = 0;    // Default reference type
  clk_cfg.core_freq = 15360000; // 15.36 MHz core frequency
  clk_cfg.voltage = 0;     // Default voltage
  
  int clock_result = syntiant_ndp120_tiny_clock_cfg(ndp, &clk_cfg);
  Serial.print("Clock config result: ");
  Serial.println(clock_result);
  
  if (clock_result == 0) {
    Serial.println("✓ Clock configuration SUCCESS!");
  } else {
    Serial.print("✗ Clock configuration FAILED with error: ");
    Serial.println(clock_result);
  }
  
  // Test audio chunk size
  Serial.println("Testing audio chunk size...");
  uint32_t chunk_size = NDP.getAudioChunkSize();
  Serial.print("Audio chunk size: ");
  Serial.println(chunk_size);
  
  if (chunk_size == 0xFFFFFFFF) {
    Serial.println("✗ Audio chunk size is 0xFFFFFFFF (DSP not ready)");
  } else if (chunk_size > 0) {
    Serial.println("✓ Audio chunk size is valid (DSP ready)");
  } else {
    Serial.println("? Audio chunk size is 0 (unexpected)");
  }
  
  Serial.println("=== Test Complete ===");
}

void loop() {
  delay(1000);
}
