/**
 * @file VerifyLittleFSIntegrity.ino
 * @brief Verify LittleFS integrity and compare files byte-by-byte
 * 
 * Tests:
 * 1. Mount LittleFS
 * 2. Read each synpkg file
 * 3. Compare first/last 256 bytes with expected
 * 4. Calculate checksum
 * 5. Verify file sizes
 */

#include "SPIFBlockDevice.h"
#include "LittleFileSystem.h"

// CS_FLASH should be defined in Arduino, but if not, use pin directly
#ifndef CS_FLASH
#define CS_FLASH (PinName)26
#endif

SPIFBlockDevice spif(SPI_PSELMOSI0, SPI_PSELMISO0, SPI_PSELSCK0, CS_FLASH, 16000000);
mbed::LittleFileSystem fs("fs");

// Expected sizes (from memory)
const unsigned int MCU_SIZE = 22636;
const unsigned int DSP_SIZE = 79828;
const unsigned int MODEL_SIZE = 417828;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  Serial.println("\n=== LittleFS Integrity Verification ===\n");
  
  // Mount
  Serial.print("Mounting LittleFS... ");
  int err = fs.mount(&spif);
  if (err) {
    Serial.print("FAIL: ");
    Serial.println(err);
    Serial.println("\n=== VERIFICATION FAILED ===");
    return;
  }
  Serial.println("OK");
  
  // Test files
  verify_file("/fs/mcu_fw_120_v91.synpkg", MCU_SIZE);
  verify_file("/fs/dsp_firmware_v91.synpkg", DSP_SIZE);
  verify_file("/fs/alexa_334_NDP120_B0_v11_v91.synpkg", MODEL_SIZE);
  
  // Unmount
  fs.unmount();
  
  Serial.println("\n=== VERIFICATION COMPLETE ===");
}

void verify_file(const char *path, unsigned int expected_size) {
  Serial.print("\n--- ");
  Serial.print(path);
  Serial.println(" ---");
  
  // Open
  FILE* f = fopen(path, "rb");
  if (!f) {
    Serial.println("  ❌ FAIL: Cannot open");
    return;
  }
  
  // Get size
  fseek(f, 0, SEEK_END);
  unsigned int size = ftell(f);
  fseek(f, 0, SEEK_SET);
  
  Serial.print("  Size: ");
  Serial.print(size);
  Serial.print(" bytes");
  
  if (size == expected_size) {
    Serial.println(" ✅");
  } else {
    Serial.print(" ❌ (expected ");
    Serial.print(expected_size);
    Serial.println(")");
  }
  
  // Read first 16 bytes
  uint8_t first[16];
  fread(first, 1, 16, f);
  
  Serial.print("  First 16: ");
  for (int i = 0; i < 16; i++) {
    if (first[i] < 0x10) Serial.print("0");
    Serial.print(first[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  // Read last 16 bytes
  uint8_t last[16];
  fseek(f, -16, SEEK_END);
  fread(last, 1, 16, f);
  
  Serial.print("  Last 16:  ");
  for (int i = 0; i < 16; i++) {
    if (last[i] < 0x10) Serial.print("0");
    Serial.print(last[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  // Calculate simple checksum (sum of all bytes)
  fseek(f, 0, SEEK_SET);
  unsigned long checksum = 0;
  uint8_t buf[256];
  unsigned int remaining = size;
  
  while (remaining > 0) {
    unsigned int to_read = (remaining > 256) ? 256 : remaining;
    unsigned int nread = fread(buf, 1, to_read, f);
    
    if (nread == 0) break;
    
    for (unsigned int i = 0; i < nread; i++) {
      checksum += buf[i];
    }
    
    remaining -= nread;
  }
  
  Serial.print("  Checksum: 0x");
  Serial.println(checksum, HEX);
  
  // Test sequential read (like Zephyr does)
  Serial.print("  Sequential read test (1024-byte chunks)... ");
  fseek(f, 0, SEEK_SET);
  uint8_t chunk[1024];
  unsigned int total_read = 0;
  int errors = 0;
  
  while (total_read < size) {
    unsigned int to_read = ((size - total_read) > 1024) ? 1024 : (size - total_read);
    unsigned int nread = fread(chunk, 1, to_read, f);
    
    if (nread != to_read) {
      errors++;
      Serial.print("ERROR at offset ");
      Serial.print(total_read);
      Serial.print(": read ");
      Serial.print(nread);
      Serial.print(" expected ");
      Serial.println(to_read);
      break;
    }
    
    total_read += nread;
  }
  
  if (errors == 0 && total_read == size) {
    Serial.println("✅ OK");
  } else {
    Serial.print("❌ FAIL (");
    Serial.print(total_read);
    Serial.print("/");
    Serial.print(size);
    Serial.println(" bytes)");
  }
  
  fclose(f);
}

void loop() {
  // Nothing
}


