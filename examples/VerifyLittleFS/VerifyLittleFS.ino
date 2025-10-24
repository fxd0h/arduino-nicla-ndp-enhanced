/**
 * @file VerifyLittleFS.ino
 * @brief Diagnostic sketch to verify LittleFS state and files
 * 
 * Upload this to Nicla Voice to check:
 * 1. LittleFS mount status
 * 2. Files present and sizes
 * 3. Superblock metadata
 * 4. Flash raw dump at offset 0
 */

#include "SPIFBlockDevice.h"
#include "LittleFileSystem.h"

SPIFBlockDevice spif(SPI_PSELMOSI0, SPI_PSELMISO0, SPI_PSELSCK0, CS_FLASH, 000000);
mbed::LittleFileSystem fs("fs");

void setup() {
  Serial.begin(115200);
  delay(3000);  // Wait for serial
  
  Serial.println("\n╔══════════════════════════════════════════════════════════╗");
  Serial.println("║         LITTLEFS VERIFICATION - Nicla Voice              ║");
  Serial.println("╚══════════════════════════════════════════════════════════╝\n");
  
  // ============================================================================
  // 1. FLASH INIT
  // ============================================================================
  Serial.println("[1] Initializing Flash...");
  int ret = spif.init();
  Serial.print("    Init result: ");
  Serial.println(ret);
  
  if (ret != 0) {
    Serial.println("    ❌ Flash init FAILED!");
    return;
  }
  
  // Get flash size
  Serial.print("    Flash size: ");
  Serial.print(spif.size() / (1024 * 1024));
  Serial.println(" MB");
  Serial.println("    ✅ Flash OK\n");
  
  // ============================================================================
  // 2. RAW FLASH DUMP (first 256 bytes - superblock)
  // ============================================================================
  Serial.println("[2] Raw Flash Dump (Offset 0x000000 - Superblock):");
  Serial.println("────────────────────────────────────────────────────────────");
  
  uint8_t raw[256];
  ret = spif.read(raw, 0, sizeof(raw));
  
  if (ret == 0) {
    // Hex dump
    for (int i = 0; i < 256; i++) {
      if (i % 16 == 0) {
        Serial.print("\n    ");
        if (i < 0x100) Serial.print("0");
        if (i < 0x10) Serial.print("0");
        Serial.print(i, HEX);
        Serial.print(": ");
      }
      if (raw[i] < 0x10) Serial.print("0");
      Serial.print(raw[i], HEX);
      Serial.print(" ");
    }
    Serial.println("\n");
    
    // Check for LFS magic
    Serial.println("    Looking for LittleFS magic bytes:");
    if (raw[0] == 0x6C && raw[1] == 0x69 && raw[2] == 0x74 && raw[3] == 0x74) {
      Serial.println("    ✅ Found 'litt' at offset 0");
    }
    
    // Look for "littlefs" string
    bool found_lfs = false;
    for (int i = 0; i < 200; i++) {
      if (memcmp(&raw[i], "littlefs", 8) == 0) {
        Serial.print("    ✅ Found 'littlefs' string at offset ");
        Serial.println(i);
        found_lfs = true;
        break;
      }
    }
    
    if (!found_lfs) {
      Serial.println("    ⚠️  'littlefs' string NOT found in first 256 bytes");
    }
    
  } else {
    Serial.println("    ❌ Flash read failed!");
  }
  Serial.println();
  
  // ============================================================================
  // 3. LITTLEFS MOUNT
  // ============================================================================
  Serial.println("[3] Mounting LittleFS...");
  ret = fs.mount(&spif);
  Serial.print("    Mount result: ");
  Serial.println(ret);
  
  if (ret != 0) {
    Serial.println("    ❌ MOUNT FAILED!");
    Serial.println("    This means flash is NOT formatted with LittleFS");
    Serial.println("    or is corrupted.\n");
    
    Serial.println("═══════════════════════════════════════════════════════════");
    Serial.println("ACTION NEEDED: Format and upload files using Arduino");
    Serial.println("═══════════════════════════════════════════════════════════");
    return;
  }
  
  Serial.println("    ✅ LittleFS mounted OK\n");
  
  // ============================================================================
  // 4. LIST FILES
  // ============================================================================
  Serial.println("[4] Files in LittleFS:");
  Serial.println("────────────────────────────────────────────────────────────");
  
  DIR *dir = opendir("/fs");
  if (dir == NULL) {
    Serial.println("    ❌ Cannot open directory!");
    return;
  }
  
  struct dirent *ent;
  int file_count = 0;
  
  while ((ent = readdir(dir)) != NULL) {
    // Skip . and ..
    if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
      continue;
    }
    
    file_count++;
    
    // Open file to get size
    String path = String("/fs/") + ent->d_name;
    FILE *f = fopen(path.c_str(), "rb");
    
    if (f) {
      fseek(f, 0, SEEK_END);
      long size = ftell(f);
      fclose(f);
      
      Serial.print("    ");
      Serial.print(file_count);
      Serial.print(". ");
      Serial.print(ent->d_name);
      Serial.print(" (");
      Serial.print(size);
      Serial.println(" bytes)");
      
      // Check if it's a synpkg file
      if (strstr(ent->d_name, ".synpkg") != NULL) {
        // Expected sizes
        if (strcmp(ent->d_name, "mcu_fw_120_v91.synpkg") == 0 && size == 22636) {
          Serial.println("       ✅ MCU firmware size correct");
        } else if (strcmp(ent->d_name, "dsp_firmware_v91.synpkg") == 0 && size == 79828) {
          Serial.println("       ✅ DSP firmware size correct");
        } else if (strstr(ent->d_name, "alexa") != NULL && size == 417828) {
          Serial.println("       ✅ Alexa model size correct");
        }
      }
    } else {
      Serial.print("    ");
      Serial.print(file_count);
      Serial.print(". ");
      Serial.print(ent->d_name);
      Serial.println(" (ERROR reading)");
    }
  }
  
  closedir(dir);
  
  Serial.println("────────────────────────────────────────────────────────────");
  Serial.print("    Total files: ");
  Serial.println(file_count);
  Serial.println();
  
  // ============================================================================
  // 5. VERIFY SYNPKG FILES
  // ============================================================================
  Serial.println("[5] Verifying synpkg files:");
  Serial.println("────────────────────────────────────────────────────────────");
  
  const char* required_files[] = {
    "/fs/mcu_fw_120_v91.synpkg",
    "/fs/dsp_firmware_v91.synpkg",
    "/fs/alexa_334_NDP120_B0_v11_v91.synpkg"
  };
  
  const long expected_sizes[] = {
    22636,
    79828,
    417828
  };
  
  int missing = 0;
  
  for (int i = 0; i < 3; i++) {
    Serial.print("    Checking: ");
    Serial.println(required_files[i]);
    
    FILE *f = fopen(required_files[i], "rb");
    if (f) {
      fseek(f, 0, SEEK_END);
      long size = ftell(f);
      fclose(f);
      
      Serial.print("      Size: ");
      Serial.print(size);
      Serial.print(" bytes (expected: ");
      Serial.print(expected_sizes[i]);
      Serial.println(")");
      
      if (size == expected_sizes[i]) {
        Serial.println("      ✅ OK");
      } else {
        Serial.println("      ⚠️  Size mismatch!");
      }
    } else {
      Serial.println("      ❌ FILE NOT FOUND!");
      missing++;
    }
    Serial.println();
  }
  
  // ============================================================================
  // 6. FILESYSTEM INFO
  // ============================================================================
  Serial.println("[6] Filesystem Info:");
  Serial.println("────────────────────────────────────────────────────────────");
  
  // LittleFS stats (if available in mbed)
  Serial.print("    Block size: ");
  Serial.print(4096);
  Serial.println(" bytes");
  
  Serial.print("    Total blocks: ");
  Serial.println(spif.size() / 4096);
  
  Serial.println();
  
  // ============================================================================
  // SUMMARY
  // ============================================================================
  Serial.println("╔══════════════════════════════════════════════════════════╗");
  
  if (missing == 0 && file_count >= 3) {
    Serial.println("║                  ✅ ALL FILES OK                          ║");
    Serial.println("║         Zephyr should be able to read files             ║");
  } else {
    Serial.println("║               ⚠️  FILES MISSING OR CORRUPT               ║");
    Serial.print("║         Missing: ");
    Serial.print(missing);
    Serial.println(" file(s)                               ║");
  }
  
  Serial.println("╚══════════════════════════════════════════════════════════╝\n");
  
  Serial.println("═══════════════════════════════════════════════════════════");
  Serial.println("NEXT STEP:");
  Serial.println("  Copy the hex dump above and share it for analysis");
  Serial.println("═══════════════════════════════════════════════════════════");
}

void loop() {
  delay(1000);
}

