/**
 * @file DebugMailboxFlow.ino
 * @brief EXHAUSTIVE debug del flujo completo de carga de firmwares
 * 
 * CRITICAL: Requiere librería Syntiant ilib modificada con DEBUG=1
 * 
 * Muestra:
 * - TODOS los registros NDP120 en cada paso
 * - Estado interno completo (pkg_load_flag, dl_state, mb_state)
 * - MBOUT después de cada operación
 * - Timing detallado
 * - Mensajes de la librería Syntiant (printf habilitado)
 */

#include "NDP.h"
#include "SPIFBlockDevice.h"
#include "LittleFileSystem.h"

SPIFBlockDevice spif(SPI_PSELMOSI0, SPI_PSELMISO0, SPI_PSELSCK0, CS_FLASH, 16000000);
mbed::LittleFileSystem fs("fs");

// Helper function declarations
void readAllNDP120Registers(const char *label);
void readNDP120StateDetailed(const char *label);
void readMBOUTDirect(uint8_t *value);
uint8_t readRegisterDirect(uint8_t reg);

void setup() {
  Serial.begin(921600);  // HIGH SPEED for verbose output
  
  // CRITICAL: Wait 5 seconds for serial monitor to connect
  Serial.println("\n\n>>> Waiting 5 seconds for serial monitor connection...");
  for (int i = 5; i > 0; i--) {
    Serial.print(">>> Starting in ");
    Serial.print(i);
    Serial.println(" seconds...");
    delay(1000);
  }
  Serial.println(">>> GO!\n");
  
  Serial.println("\n╔══════════════════════════════════════════════════════════════════╗");
  Serial.println("║    NDP120 EXHAUSTIVE DEBUG - Internal State & Registers         ║");
  Serial.println("║    Serial: 921600 baud, Syntiant ilib DEBUG ENABLED             ║");
  Serial.println("╚══════════════════════════════════════════════════════════════════╝\n");
  
  Serial.println("NOTE: Modified libraries:");
  Serial.println("  - NDP: ndp/ndp exposed (backup in NDP_BACKUP_ORIGINAL)");
  Serial.println("  - syntiant_ilib: SYNTIANT_NDP120_DEBUG=1 (verbose output)\n");
  
  Serial.println("═══════════════════════════════════════════════════════════════════");
  Serial.println("INITIAL STATE - Before NDP.begin()");
  Serial.println("═══════════════════════════════════════════════════════════════════\n");
  
  Serial.println("NOTE: NDP.begin() will do GPIO reset + init internally");
  
  // ============================================================================
  // PHASE 1: NDP.begin() - MCU Firmware Load
  // ============================================================================
  Serial.println("═══════════════════════════════════════════════════════════════════");
  Serial.println("PHASE 1: NDP.begin() - MCU Firmware via Bootloader");
  Serial.println("═══════════════════════════════════════════════════════════════════\n");
  
  Serial.println("[BEGIN] Starting NDP.begin(\"mcu_fw_120_v91.synpkg\")...\n");
  Serial.println("--- Syntiant Library Output (printf enabled) ---");
  
  unsigned long mcu_start = millis();
  NDP.begin("mcu_fw_120_v91.synpkg");
  unsigned long mcu_time = millis() - mcu_start;
  
  Serial.println("\n--- End of Syntiant Library Output ---\n");
  
  Serial.print("[BEGIN] Completed in ");
  Serial.print(mcu_time);
  Serial.println(" ms\n");
  
  // Read ALL state after MCU load
  readAllNDP120Registers("AFTER_MCU_LOAD");
  readNDP120StateDetailed("AFTER_MCU_LOAD");
  Serial.println();
  
  // ============================================================================
  // PHASE 2: DSP Firmware Load
  // ============================================================================
  Serial.println("═══════════════════════════════════════════════════════════════════");
  Serial.println("PHASE 2: DSP Firmware Load");
  Serial.println("═══════════════════════════════════════════════════════════════════\n");
  
  Serial.println("[DSP] Loading dsp_firmware_v91.synpkg...\n");
  Serial.println("--- Syntiant Library Output ---");
  
  unsigned long dsp_start = millis();
  NDP.load("dsp_firmware_v91.synpkg", 0);
  unsigned long dsp_time = millis() - dsp_start;
  
  Serial.println("\n--- End of Syntiant Library Output ---\n");
  
  Serial.print("[DSP] Completed in ");
  Serial.print(dsp_time);
  Serial.println(" ms\n");
  
  // Read ALL state after DSP load
  readAllNDP120Registers("AFTER_DSP_LOAD");
  readNDP120StateDetailed("AFTER_DSP_LOAD");
  Serial.println();
  
  // ============================================================================
  // POST-DSP: Delay + Poll + Clock Config
  // ============================================================================
  Serial.println("═══════════════════════════════════════════════════════════════════");
  Serial.println("POST-DSP: Stabilization");
  Serial.println("═══════════════════════════════════════════════════════════════════\n");
  
  Serial.println("[POST-DSP] Waiting 750ms for DSP stabilization...");
  delay(750);
  
  readAllNDP120Registers("AFTER_750MS_DELAY");
  
  Serial.println("\n[POST-DSP] Calling NDP.poll()...\n");
  Serial.println("--- Syntiant Library Output ---");
  NDP.poll();
  Serial.println("--- End of Syntiant Library Output ---\n");
  
  readAllNDP120Registers("AFTER_POLL");
  readNDP120StateDetailed("AFTER_POLL");
  
  Serial.println("\n[POST-DSP] Calling NDP.configureClock()...\n");
  Serial.println("--- Syntiant Library Output ---");
  NDP.configureClock();
  Serial.println("--- End of Syntiant Library Output ---\n");
  
  readAllNDP120Registers("AFTER_CLOCK_CFG");
  readNDP120StateDetailed("AFTER_CLOCK_CFG");
  
  Serial.println("\n[POST-DSP] Increasing SPI speed to 12MHz...");
  SPI1.endTransaction();
  SPI1.beginTransaction(SPISettings(12000000, MSBFIRST, SPI_MODE0));
  Serial.println();
  
  // ============================================================================
  // PHASE 3: NN Model Load
  // ============================================================================
  Serial.println("═══════════════════════════════════════════════════════════════════");
  Serial.println("PHASE 3: NN Model Load");
  Serial.println("═══════════════════════════════════════════════════════════════════\n");
  
  Serial.println("[MODEL] Loading alexa_334_NDP120_B0_v11_v91.synpkg...\n");
  Serial.println("--- Syntiant Library Output ---");
  
  unsigned long model_start = millis();
  NDP.load("alexa_334_NDP120_B0_v11_v91.synpkg", 0);
  unsigned long model_time = millis() - model_start;
  
  Serial.println("\n--- End of Syntiant Library Output ---\n");
  
  Serial.print("[MODEL] Completed in ");
  Serial.print(model_time);
  Serial.println(" ms\n");
  
  // Read ALL state after Model load
  readAllNDP120Registers("AFTER_MODEL_LOAD");
  readNDP120StateDetailed("AFTER_MODEL_LOAD");
  Serial.println();
  
  // ============================================================================
  // FINAL TESTS
  // ============================================================================
  Serial.println("═══════════════════════════════════════════════════════════════════");
  Serial.println("FINAL TESTS");
  Serial.println("═══════════════════════════════════════════════════════════════════\n");
  
  Serial.println("[TEST] getAudioChunkSize()...");
  int chunk_size = NDP.getAudioChunkSize();
  Serial.print("[TEST] Result: ");
  Serial.println(chunk_size);
  
  if (chunk_size > 0 && chunk_size < 100000) {
    Serial.println("[TEST] ✅ DSP is WORKING!");
  } else {
    Serial.print("[TEST] ❌ DSP not responding (invalid: ");
    Serial.print(chunk_size, HEX);
    Serial.println(")");
  }
  Serial.println();
  
  readAllNDP120Registers("FINAL");
  readNDP120StateDetailed("FINAL");
  
  // ============================================================================
  // SUMMARY
  // ============================================================================
  Serial.println("\n═══════════════════════════════════════════════════════════════════");
  Serial.println("TIMING SUMMARY");
  Serial.println("═══════════════════════════════════════════════════════════════════");
  Serial.print("  MCU firmware:  ");
  Serial.print(mcu_time);
  Serial.println(" ms");
  Serial.print("  DSP firmware:  ");
  Serial.print(dsp_time);
  Serial.println(" ms");
  Serial.print("  NN Model:      ");
  Serial.print(model_time);
  Serial.println(" ms");
  Serial.print("  TOTAL:         ");
  Serial.print(mcu_time + dsp_time + model_time);
  Serial.println(" ms");
  Serial.println("═══════════════════════════════════════════════════════════════════\n");
  
  Serial.println("╔══════════════════════════════════════════════════════════════════╗");
  Serial.println("║                     ANALYSIS COMPLETE                            ║");
  Serial.println("╚══════════════════════════════════════════════════════════════════╝");
}

void loop() {
  // Nothing
}

/**
 * Read ALL NDP120 registers and display
 */
void readAllNDP120Registers(const char *label) {
  Serial.print("┌──────────────────────────────────────────────────────────────┐\n");
  Serial.print("│ NDP120 Registers @ ");
  Serial.print(label);
  Serial.println();
  Serial.print("└──────────────────────────────────────────────────────────────┘\n");
  
  // Read all important registers
  uint8_t id0 = readRegisterDirect(0x00);
  uint8_t id1 = readRegisterDirect(0x01);
  uint8_t intsts = readRegisterDirect(0x02);
  uint8_t matchsts = readRegisterDirect(0x03);
  uint8_t mbout = readRegisterDirect(0x04);
  uint8_t mbin = readRegisterDirect(0x05);
  uint8_t intctl = readRegisterDirect(0x10);
  uint8_t ctl = readRegisterDirect(0x11);
  uint8_t cfg = readRegisterDirect(0x12);
  
  Serial.println("  Register Map:");
  Serial.print("    ID0     (0x00): 0x");
  Serial.print(id0, HEX);
  Serial.println(id0 == 0x38 ? " ✅ (NDP120)" : " ❌");
  
  Serial.print("    ID1     (0x01): 0x");
  Serial.println(id1, HEX);
  
  Serial.print("    INTSTS  (0x02): 0x");
  Serial.print(intsts, HEX);
  Serial.print(" (");
  if (intsts & 0x10) Serial.print("MBOUT ");
  if (intsts & 0x08) Serial.print("MBIN ");
  if (intsts & 0x04) Serial.print("WM ");
  if (intsts & 0x02) Serial.print("MATCH ");
  if (intsts & 0x01) Serial.print("AE ");
  Serial.println(")");
  
  Serial.print("    MATCHSTS(0x03): 0x");
  Serial.println(matchsts, HEX);
  
  Serial.print("    MBOUT   (0x04): 0x");
  Serial.print(mbout, HEX);
  if (mbout == 0x12) Serial.println(" (LOAD_DONE)");
  else if (mbout == 0x14) Serial.println(" (OTHER_LOAD_DONE)");
  else if (mbout == 0x11) Serial.println(" (BOOTING)");
  else if (mbout == 0x10) Serial.println(" (RUNNING)");
  else if (mbout == 0x00) Serial.println(" (empty)");
  else Serial.println();
  
  Serial.print("    MBIN    (0x05): 0x");
  Serial.print(mbin, HEX);
  Serial.print(" (seq bit: ");
  Serial.print((mbin & 0x80) ? "1" : "0");
  Serial.println(")");
  
  Serial.print("    INTCTL  (0x10): 0x");
  Serial.print(intctl, HEX);
  Serial.print(" (");
  if (intctl & 0x80) Serial.print("RF ");
  if (intctl & 0x40) Serial.print("WM ");
  if (intctl & 0x20) Serial.print("FEATURE ");
  if (intctl & 0x10) Serial.print("DNN ");
  if (intctl & 0x08) Serial.print("MBOUT ");
  if (intctl & 0x04) Serial.print("MBIN ");
  if (intctl & 0x02) Serial.print("MATCH ");
  if (intctl & 0x01) Serial.print("AE ");
  Serial.println(")");
  
  Serial.print("    CTL     (0x11): 0x");
  Serial.print(ctl, HEX);
  Serial.print(" (");
  if (ctl & 0x04) Serial.print("CLKEN ");
  if (ctl & 0x02) Serial.print("PORSTN ");
  if (ctl & 0x01) Serial.print("RESETN ");
  Serial.print("FLASHCTL=");
  Serial.print((ctl >> 6) & 0x03);
  Serial.println(")");
  
  Serial.print("    CFG     (0x12): 0x");
  Serial.print(cfg, HEX);
  Serial.print(" (INTEN=");
  Serial.print((cfg & 0x80) ? "1" : "0");
  Serial.print(" INTNEG=");
  Serial.print((cfg & 0x40) ? "1" : "0");
  Serial.println(")");
  Serial.println();
}

/**
 * Read detailed internal state from ndp structure
 */
void readNDP120StateDetailed(const char *label) {
  Serial.print("┌──────────────────────────────────────────────────────────────┐\n");
  Serial.print("│ Internal State @ ");
  Serial.print(label);
  Serial.println();
  Serial.print("└──────────────────────────────────────────────────────────────┘\n");
  
  // pkg_load_flag
  Serial.print("  pkg_load_flag: 0x");
  Serial.print(ndp->pkg_load_flag, HEX);
  Serial.print(" (");
  if (ndp->pkg_load_flag == 0x00) Serial.print("NONE");
  if (ndp->pkg_load_flag & 0x01) Serial.print("MCU ");
  if (ndp->pkg_load_flag & 0x02) Serial.print("DSP ");
  if (ndp->pkg_load_flag & 0x04) Serial.print("NN ");
  Serial.println(")");
  
  // dl_state
  Serial.print("  dl_state.mode: ");
  Serial.print(ndp->dl_state.mode);
  switch(ndp->dl_state.mode) {
    case 0: Serial.println(" (COMPLETE)"); break;
    case 1: Serial.println(" (START)"); break;
    case 2: Serial.println(" (IN_PROGRESS)"); break;
    default: Serial.println(" (UNKNOWN)"); break;
  }
  
  Serial.print("  dl_state.window_lower: 0x");
  Serial.println(ndp->dl_state.window_lower, HEX);
  Serial.print("  dl_state.window_upper: 0x");
  Serial.println(ndp->dl_state.window_upper, HEX);
  Serial.print("  dl_state.window_idx: ");
  Serial.println(ndp->dl_state.window_idx);
  Serial.print("  dl_state.remainder_len: ");
  Serial.println(ndp->dl_state.remainder_len);
  
  // mb_state
  Serial.print("  mb_state.mbout: 0x");
  Serial.println(ndp->mb_state.mbout, HEX);
  Serial.print("  mb_state.mbin_seq: 0x");
  Serial.println(ndp->mb_state.mbin_seq, HEX);
  Serial.print("  mb_state.mbin_state: ");
  Serial.println(ndp->mb_state.mbin_state);
  Serial.print("  mb_state.mbin_sync_count: ");
  Serial.println(ndp->mb_state.mbin_sync_count);
  Serial.print("  mb_state.watermarkint: 0x");
  Serial.println(ndp->mb_state.watermarkint, HEX);
  
  // Other critical fields
  Serial.print("  dsp_fw_state_addr: 0x");
  Serial.println(ndp->dsp_fw_state_addr, HEX);
  Serial.print("  tankptr_last: 0x");
  Serial.println(ndp->tankptr_last, HEX);
  Serial.print("  tankptr_match: 0x");
  Serial.println(ndp->tankptr_match, HEX);
  
  Serial.println();
}

/**
 * Read single NDP120 register directly via SPI
 */
uint8_t readRegisterDirect(uint8_t reg) {
  uint8_t cmd = 0x80 | reg;  // Read command
  uint8_t value = 0;
  
  digitalWrite(NDPClass::NDP_CS, LOW);
  SPI1.transfer(cmd);
  value = SPI1.transfer(0xFF);
  digitalWrite(NDPClass::NDP_CS, HIGH);
  
  return value;
}

/**
 * Read MBOUT register directly via SPI
 */
void readMBOUTDirect(uint8_t *value) {
  *value = readRegisterDirect(0x04);
}
