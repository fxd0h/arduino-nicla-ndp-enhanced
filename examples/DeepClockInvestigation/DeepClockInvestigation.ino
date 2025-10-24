/**
 * @file DeepClockInvestigation.ino
 * @brief Deep investigation of NDP120 Clock Configuration
 * 
 * This sketch will show us EXACTLY what happens during clock config
 * and why it works in Arduino but fails in Zephyr.
 */

#include <NDP.h>

void setup() {
  Serial.begin(115200);
  delay(3000);  // Wait for serial monitor
  
  Serial.println("=== DEEP CLOCK INVESTIGATION ===");
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
  
  // Now we're at the EXACT same point as Zephyr
  // Let's investigate the state before clock config
  Serial.println("\n[3] Investigating NDP120 state before clock config...");
  
  // Get the internal ndp structure for debugging
  extern struct syntiant_ndp120_tiny_device_s *ndp;
  if (ndp == NULL) {
    Serial.println("ERROR: ndp structure not accessible!");
    return;
  }
  
  Serial.println("NDP120 Internal State:");
  Serial.print("  pkg_load_flag: 0x");
  Serial.println(ndp->pkg_load_flag, HEX);
  Serial.print("  mb_state.mbout: 0x");
  Serial.println(ndp->mb_state.mbout, HEX);
  Serial.print("  mb_state.mbin_resp: 0x");
  Serial.println(ndp->mb_state.mbin_resp, HEX);
  Serial.print("  mb_state.mbin_seq: 0x");
  Serial.println(ndp->mb_state.mbin_seq, HEX);
  Serial.print("  mb_state.mbout_seq: 0x");
  Serial.println(ndp->mb_state.mbout_seq, HEX);
  Serial.print("  init: ");
  Serial.println(ndp->init);
  Serial.print("  mcu_fw_pointers_addr: 0x");
  Serial.println(ndp->mcu_fw_pointers_addr, HEX);
  Serial.print("  dsp_fw_state_addr: 0x");
  Serial.println(ndp->dsp_fw_state_addr, HEX);
  
  // Test polling before clock config
  Serial.println("\n[4] Testing polling before clock config...");
  uint32_t notifications = 0;
  int poll_result = syntiant_ndp120_tiny_poll(ndp, &notifications, 1);
  Serial.print("Poll result: ");
  Serial.println(poll_result);
  Serial.print("Notifications: 0x");
  Serial.println(notifications, HEX);
  
  // Test mailbox commands before clock config
  Serial.println("\n[5] Testing mailbox commands before clock config...");
  uint32_t msg = 0;
  int mb_result = syntiant_ndp120_tiny_mb_cmd(ndp, SYNTIANT_NDP120_MB_MCU_NOP, &msg);
  Serial.print("MB NOP result: ");
  Serial.println(mb_result);
  Serial.print("MB NOP message: 0x");
  Serial.println(msg, HEX);
  
  // Test DSP address commands
  mb_result = syntiant_ndp120_tiny_mb_cmd(ndp, SYNTIANT_NDP120_MB_DSP_ADX_LOWER, &msg);
  Serial.print("MB DSP_ADX_LOWER result: ");
  Serial.println(mb_result);
  Serial.print("MB DSP_ADX_LOWER message: 0x");
  Serial.println(msg, HEX);
  
  // Now test clock configuration with detailed logging
  Serial.println("\n[6] Testing Clock Configuration with detailed logging...");
  
  // Show the exact parameters we're using
  struct syntiant_ndp120_tiny_clk_config_data clk_cfg;
  clk_cfg.src = SYNTIANT_NDP120_MAIN_CLK_SRC_FLL;  // 3
  clk_cfg.ref_freq = 32768;                         // 32.768 kHz
  clk_cfg.ref_type = 0;                             // clk_pad
  clk_cfg.core_freq = 15360000;                     // 15.36 MHz
  clk_cfg.voltage = 0;                              // 0.9V
  
  Serial.println("Clock config parameters:");
  Serial.print("  src: ");
  Serial.println(clk_cfg.src);
  Serial.print("  ref_freq: ");
  Serial.println(clk_cfg.ref_freq);
  Serial.print("  ref_type: ");
  Serial.println(clk_cfg.ref_type);
  Serial.print("  core_freq: ");
  Serial.println(clk_cfg.core_freq);
  Serial.print("  voltage: ");
  Serial.println(clk_cfg.voltage);
  
  // Call clock configuration
  Serial.println("\nCalling syntiant_ndp120_tiny_clock_cfg()...");
  int clock_result = syntiant_ndp120_tiny_clock_cfg(ndp, &clk_cfg);
  Serial.print("Clock config result: ");
  Serial.println(clock_result);
  
  if (clock_result == 0) {
    Serial.println("✓ Clock configuration SUCCESS!");
  } else {
    Serial.print("✗ Clock configuration FAILED with error: ");
    Serial.println(clock_result);
    
    // Show error name
    const char* error_names[] = {
      "none", "fail", "arg", "uninit", "package", "unsup", "nomem", "busy",
      "timeout", "more", "config", "crc", "inv_net", "reread", "pbi_tag",
      "pbi_ver", "invalid_length", "dsp_hdr_crc"
    };
    
    if (clock_result >= 0 && clock_result < 17) {
      Serial.print("Error name: ");
      Serial.println(error_names[clock_result]);
    }
    
    // Show state after failed clock config
    Serial.println("\nState after failed clock config:");
    Serial.print("  pkg_load_flag: 0x");
    Serial.println(ndp->pkg_load_flag, HEX);
    Serial.print("  mb_state.mbout: 0x");
    Serial.println(ndp->mb_state.mbout, HEX);
    Serial.print("  mb_state.mbin_resp: 0x");
    Serial.println(ndp->mb_state.mbin_resp, HEX);
  }
  
  // Test if we can get audio chunk size
  Serial.println("\n[7] Testing Audio Chunk Size...");
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
  
  // Test PDM clock mode
  Serial.println("\n[8] Testing PDM Clock Mode...");
  int pdm_result = syntiant_ndp120_tiny_pdm_clock_exe_mode(ndp, SYNTIANT_NDP120_PDM_CLK_START_CLEAN);
  Serial.print("PDM clock result: ");
  Serial.println(pdm_result);
  
  if (pdm_result == 0) {
    Serial.println("✓ PDM clock started successfully");
  } else {
    Serial.print("✗ PDM clock failed with error: ");
    Serial.println(pdm_result);
  }
  
  Serial.println("\n=== INVESTIGATION COMPLETE ===");
}

void loop() {
  // Nothing to do in loop
  delay(1000);
}
