/**
 * @file CheckLittleFSVersion.ino
 * @brief Check which version of LittleFS is on the flash (v1 or v2)
 * 
 * Reads superblock metadata directly from flash to determine format version
 */

#include "SPIFBlockDevice.h"

SPIFBlockDevice spif(SPI_PSELMOSI0, SPI_PSELMISO0, SPI_PSELSCK0, CS_FLASH, 16000000);

void setup() {
  Serial.begin(115200);
  delay(3000);
  
  Serial.println("\n╔══════════════════════════════════════════════════════════╗");
  Serial.println("║         LITTLEFS VERSION DETECTOR                       ║");
  Serial.println("╚══════════════════════════════════════════════════════════╝\n");
  
  // ============================================================================
  // 1. INIT FLASH
  // ============================================================================
  Serial.println("[1] Initializing flash...");
  int ret = spif.init();
  if (ret != 0) {
    Serial.print("    ❌ Flash init failed: ");
    Serial.println(ret);
    return;
  }
  Serial.println("    ✅ Flash OK\n");
  
  // ============================================================================
  // 2. READ SUPERBLOCK (first 512 bytes)
  // ============================================================================
  Serial.println("[2] Reading superblock...");
  uint8_t superblock[512];
  ret = spif.read(superblock, 0, sizeof(superblock));
  
  if (ret != 0) {
    Serial.println("    ❌ Failed to read superblock");
    return;
  }
  Serial.println("    ✅ Superblock read OK\n");
  
  // ============================================================================
  // 3. HEX DUMP (first 128 bytes)
  // ============================================================================
  Serial.println("[3] Superblock hex dump (first 128 bytes):");
  Serial.println("────────────────────────────────────────────────────────────");
  for (int i = 0; i < 128; i++) {
    if (i % 16 == 0) {
      Serial.print("\n    ");
      if (i < 0x100) Serial.print("0");
      if (i < 0x10) Serial.print("0");
      Serial.print(i, HEX);
      Serial.print(": ");
    }
    if (superblock[i] < 0x10) Serial.print("0");
    Serial.print(superblock[i], HEX);
    Serial.print(" ");
  }
  Serial.println("\n");
  
  // ============================================================================
  // 4. DETECT VERSION
  // ============================================================================
  Serial.println("[4] Detecting LittleFS version:");
  Serial.println("────────────────────────────────────────────────────────────");
  
  // Check for "littlefs" magic string (common to both versions)
  bool found_littlefs = false;
  int magic_offset = -1;
  
  for (int i = 0; i < 256; i++) {
    if (memcmp(&superblock[i], "littlefs", 8) == 0) {
      found_littlefs = true;
      magic_offset = i;
      Serial.print("    ✅ Found 'littlefs' magic at offset ");
      Serial.println(i);
      break;
    }
  }
  
  if (!found_littlefs) {
    Serial.println("    ❌ No LittleFS magic found!");
    Serial.println("    ⚠️  Flash may not be formatted with LittleFS\n");
    return;
  }
  
  // LittleFS v1 superblock structure (offset 0):
  // - 0x00-0x03: Magic (0x6C 0x69 0x74 0x74) = "litt"
  // - 0x04-0x07: Version (0x00010001 for v1.0.1, 0x00010007 for v1.0.7)
  // - 0x08-0x0B: Block size
  // - 0x0C-0x0F: Block count
  
  // LittleFS v2 superblock structure (offset 0):
  // - Different magic and structure
  
  // Check version bytes after magic
  if (magic_offset >= 0) {
    // Read 4-byte version (little-endian)
    uint32_t version = superblock[magic_offset - 4] | 
                      (superblock[magic_offset - 3] << 8) |
                      (superblock[magic_offset - 2] << 16) |
                      (superblock[magic_offset - 1] << 24);
    
    Serial.print("    Version bytes at offset ");
    Serial.print(magic_offset - 4);
    Serial.print(": 0x");
    if (version < 0x10000000) Serial.print("0");
    if (version < 0x1000000) Serial.print("0");
    if (version < 0x100000) Serial.print("0");
    if (version < 0x10000) Serial.print("0");
    if (version < 0x1000) Serial.print("0");
    if (version < 0x100) Serial.print("0");
    if (version < 0x10) Serial.print("0");
    Serial.println(version, HEX);
    
    // Interpret version
    if (version == 0x00010001) {
      Serial.println("\n    📦 LittleFS v1.0.1 (v1 family)");
    } else if (version == 0x00010007) {
      Serial.println("\n    📦 LittleFS v1.0.7 / v1.7 (v1 family)");
    } else if (version == 0x00020000 || version == 0x00020001) {
      Serial.println("\n    📦 LittleFS v2.x (v2 family) ❌");
    } else {
      Serial.print("\n    ⚠️  Unknown version: 0x");
      Serial.println(version, HEX);
    }
  }
  
  // Additional check: block size and block count
  if (magic_offset == 40) {
    // v1 format detected (magic at offset 40)
    Serial.println("\n    Magic at offset 40 → LittleFS v1 format ✅");
    
    // Read block_size and block_count (at offset 8-11 and 12-15)
    uint32_t block_size = superblock[8] | (superblock[9] << 8) | 
                         (superblock[10] << 16) | (superblock[11] << 24);
    uint32_t block_count = superblock[12] | (superblock[13] << 8) | 
                          (superblock[14] << 16) | (superblock[15] << 24);
    
    Serial.print("    Block size: ");
    Serial.print(block_size);
    Serial.println(" bytes");
    
    Serial.print("    Block count: ");
    Serial.println(block_count);
    
    Serial.print("    Total capacity: ");
    Serial.print((block_size * block_count) / (1024 * 1024));
    Serial.println(" MB");
  } else if (magic_offset < 20) {
    Serial.println("\n    Magic at early offset → Likely LittleFS v2 ❌");
  }
  
  // ============================================================================
  // 5. SUMMARY
  // ============================================================================
  Serial.println("\n╔══════════════════════════════════════════════════════════╗");
  
  if (magic_offset == 40) {
    Serial.println("║              ✅ LITTLEFS V1 DETECTED                     ║");
    Serial.println("║         Compatible with Zephyr (v1.7.2)                 ║");
  } else if (magic_offset >= 0 && magic_offset < 20) {
    Serial.println("║              ❌ LITTLEFS V2 DETECTED                     ║");
    Serial.println("║         NOT compatible with Zephyr (needs v1)           ║");
    Serial.println("║                                                          ║");
    Serial.println("║  ACTION: Run FormatLittleFS_V1 sketch to reformat       ║");
  } else {
    Serial.println("║              ⚠️  UNKNOWN FORMAT                          ║");
  }
  
  Serial.println("╚══════════════════════════════════════════════════════════╝\n");
  
  // ============================================================================
  // 6. DETAILED ANALYSIS
  // ============================================================================
  Serial.println("[6] Detailed superblock analysis:");
  Serial.println("────────────────────────────────────────────────────────────");
  
  Serial.println("\n    First 64 bytes as uint32_t (little-endian):");
  for (int i = 0; i < 64; i += 4) {
    uint32_t val = superblock[i] | (superblock[i+1] << 8) | 
                   (superblock[i+2] << 16) | (superblock[i+3] << 24);
    Serial.print("      [");
    if (i < 10) Serial.print("0");
    Serial.print(i);
    Serial.print("] = 0x");
    if (val < 0x10000000) Serial.print("0");
    if (val < 0x1000000) Serial.print("0");
    if (val < 0x100000) Serial.print("0");
    if (val < 0x10000) Serial.print("0");
    if (val < 0x1000) Serial.print("0");
    if (val < 0x100) Serial.print("0");
    if (val < 0x10) Serial.print("0");
    Serial.print(val, HEX);
    Serial.print("  (");
    Serial.print(val);
    Serial.println(")");
  }
  
  Serial.println("\n✅ Analysis complete!");
}

void loop() {
  delay(1000);
}

