/**
 * @file DebugClockConfig.ino
 * @brief Debug NDP120 Clock Configuration
 * 
 * This sketch tests the clock configuration that's failing in Zephyr.
 * We'll see exactly what syntiant_ndp120_tiny_clock_cfg() does and if it works.
 */

#include <NDP.h>

void setup() {
  Serial.begin(921600);
  delay(5000);  // Wait for serial monitor
  
  Serial.println("=== NDP120 Clock Configuration Debug ===");
  Serial.println("Build: " __DATE__ " " __TIME__);
  Serial.println();
  
  // Initialize NDP120 with MCU firmware
  Serial.println("[1] Initializing NDP120...");
  if (!NDP.begin("mcu_fw_120_v91.synpkg")) {
    Serial.println("ERROR: NDP120 initialization failed!");
    return;
  }
  Serial.println("✓ NDP120 initialized with MCU firmware");
  
  // Load DSP firmware
  Serial.println("\n[2] Loading DSP firmware...");
  if (!NDP.load("dsp_firmware_v91.synpkg")) {
    Serial.println("ERROR: DSP firmware load failed!");
    return;
  }
  Serial.println("✓ DSP firmware loaded");
  
  // Now test clock configuration
  Serial.println("\n[3] Testing Clock Configuration...");
  Serial.println("Calling syntiant_ndp120_tiny_clock_cfg()...");
  
  // Get the internal ndp structure for debugging
  extern struct syntiant_ndp120_tiny_device_s *ndp;
  if (ndp == NULL) {
    Serial.println("ERROR: ndp structure not accessible!");
    return;
  }
  
  Serial.println("Before clock_cfg:");
  Serial.print("  pkg_load_flag: 0x");
  Serial.println(ndp->pkg_load_flag, HEX);
  Serial.print("  mb_state.mbout: 0x");
  Serial.println(ndp->mb_state.mbout, HEX);
  
  // Call clock configuration with proper parameters
  // We need to provide clock config data
  struct syntiant_ndp120_tiny_clk_config_data clk_cfg;
  clk_cfg.src = 3;         // FLL source (SYNTIANT_NDP120_MAIN_CLK_SRC_FLL)
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
    
    // Try to get more details about the error
    Serial.println("Error details:");
    Serial.print("  pkg_load_flag: 0x");
    Serial.println(ndp->pkg_load_flag, HEX);
    Serial.print("  mb_state.mbout: 0x");
    Serial.println(ndp->mb_state.mbout, HEX);
  }
  
  // Test if we can get audio chunk size (this should work if clock is OK)
  Serial.println("\n[4] Testing Audio Chunk Size...");
  uint32_t chunk_size = NDP.getAudioChunkSize();
  Serial.print("Audio chunk size: ");
  Serial.print(chunk_size);
  Serial.print(" (0x");
  Serial.print(chunk_size, HEX);
  Serial.println(")");
  
  if (chunk_size == 0xFFFFFFFF) {
    Serial.println("✗ Audio chunk size is 0xFFFFFFFF (DSP not ready)");
  } else if (chunk_size > 0) {
    Serial.println("✓ Audio chunk size is valid (DSP ready)");
  } else {
    Serial.println("? Audio chunk size is 0 (unexpected)");
  }
  
  Serial.println("\n=== Debug Complete ===");
}

void loop() {
  // Nothing to do in loop
  delay(1000);
}
