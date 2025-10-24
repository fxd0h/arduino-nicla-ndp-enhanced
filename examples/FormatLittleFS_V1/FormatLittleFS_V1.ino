/**
 * @file FormatLittleFS_V1.ino
 * @brief Format flash with LittleFS v1 (NOT v2) and upload synpkg files
 * 
 * CRITICAL: Must use LittleFileSystem (v1), NOT LittleFileSystem2 (v2)
 * 
 * Zephyr uses LittleFS v1.7.2, so Arduino MUST also use v1!
 */

#include "SPIFBlockDevice.h"
#include "LittleFileSystem.h"  // v1 - NOT LittleFileSystem2!

SPIFBlockDevice spif(SPI_PSELMOSI0, SPI_PSELMISO0, SPI_PSELSCK0, CS_FLASH, 16000000);
mbed::LittleFileSystem fs("fs");  // v1

void setup() {
  Serial.begin(115200);
  delay(3000);
  
  Serial.println("\n╔══════════════════════════════════════════════════════════╗");
  Serial.println("║   FORMAT FLASH WITH LITTLEFS V1 (for Zephyr compat)     ║");
  Serial.println("╚══════════════════════════════════════════════════════════╝\n");
  
  Serial.println("⚠️  WARNING: This will ERASE all data on flash!");
  Serial.println("    Press any key to continue or reset to abort...\n");
  
  while (!Serial.available()) {
    delay(100);
  }
  
  // Clear input
  while (Serial.available()) {
    Serial.read();
  }
  
  Serial.println("\n[1] Initializing flash...");
  int ret = spif.init();
  if (ret != 0) {
    Serial.print("    ❌ Flash init failed: ");
    Serial.println(ret);
    return;
  }
  Serial.println("    ✅ Flash initialized");
  
  // Unmount if mounted
  fs.unmount();
  
  Serial.println("\n[2] Formatting flash with LittleFS v1...");
  Serial.println("    This will take ~30 seconds...");
  
  ret = fs.format(&spif);
  if (ret != 0) {
    Serial.print("    ❌ Format failed: ");
    Serial.println(ret);
    return;
  }
  Serial.println("    ✅ Format complete!");
  
  Serial.println("\n[3] Mounting filesystem...");
  ret = fs.mount(&spif);
  if (ret != 0) {
    Serial.print("    ❌ Mount failed: ");
    Serial.println(ret);
    return;
  }
  Serial.println("    ✅ Mounted OK");
  
  Serial.println("\n[4] Verifying empty filesystem...");
  DIR *dir = opendir("/fs");
  if (dir == NULL) {
    Serial.println("    ❌ Cannot open directory");
    return;
  }
  
  struct dirent *ent;
  int file_count = 0;
  while ((ent = readdir(dir)) != NULL) {
    if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
      file_count++;
    }
  }
  closedir(dir);
  
  if (file_count == 0) {
    Serial.println("    ✅ Filesystem is empty (OK)");
  } else {
    Serial.print("    ⚠️  Found ");
    Serial.print(file_count);
    Serial.println(" files (unexpected after format)");
  }
  
  Serial.println("\n╔══════════════════════════════════════════════════════════╗");
  Serial.println("║                 ✅ FORMAT COMPLETE                        ║");
  Serial.println("║                                                          ║");
  Serial.println("║  NEXT STEP: Upload synpkg files to /fs/                 ║");
  Serial.println("║                                                          ║");
  Serial.println("║  Required files:                                         ║");
  Serial.println("║    - /fs/mcu_fw_120_v91.synpkg          (22636 bytes)   ║");
  Serial.println("║    - /fs/dsp_firmware_v91.synpkg        (79828 bytes)   ║");
  Serial.println("║    - /fs/alexa_334_NDP120_B0_v11_v91.synpkg             ║");
  Serial.println("║                                      (417828 bytes)     ║");
  Serial.println("╚══════════════════════════════════════════════════════════╝\n");
  
  Serial.println("📋 You can now use the Arduino IDE Serial Upload tool");
  Serial.println("   or run the UploadSynpkg sketch to upload files.\n");
}

void loop() {
  delay(1000);
}

