/*
  LittleFS Diagnostic Tool - Quick File List
*/

#include "SPIFBlockDevice.h"
#include "LittleFileSystem.h"

SPIFBlockDevice spif(SPI_PSELMOSI0, SPI_PSELMISO0, SPI_PSELSCK0, CS_FLASH, 16000000);
mbed::LittleFileSystem fs("fs");

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n=== LITTLEFS FILE LIST ===");
  
  // Initialize and mount
  spif.init();
  int ret = fs.mount(&spif);
  Serial.print("Mount result: ");
  Serial.println(ret);
  
  if (ret == 0) {
    Serial.println("\n📂 Files in /fs/:");
    Serial.println("------------------");
    
    DIR *dir = opendir("/fs");
    if (dir != NULL) {
      struct dirent *ent;
      int count = 0;
      while ((ent = readdir(dir)) != NULL) {
        // Skip . and ..
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
          continue;
        }
        
        count++;
        Serial.print(count);
        Serial.print(". ");
        Serial.print(ent->d_name);
        
        // Get file size
        String path = String("/fs/") + ent->d_name;
        FILE *f = fopen(path.c_str(), "rb");
        if (f) {
          fseek(f, 0, SEEK_END);
          long size = ftell(f);
          fclose(f);
          Serial.print(" (");
          Serial.print(size);
          Serial.println(" bytes)");
        } else {
          Serial.println(" (error reading)");
        }
      }
      closedir(dir);
      
      Serial.println("------------------");
      Serial.print("Total files: ");
      Serial.println(count);
      
      if (count == 3) {
        Serial.println("✅ ALL 3 FILES PRESENT!");
      } else {
        Serial.print("⚠️  MISSING FILES! Expected 3, found ");
        Serial.println(count);
      }
    } else {
      Serial.println("ERROR: Cannot open directory");
    }
    
  } else {
    Serial.println("❌ MOUNT FAILED");
  }
  
  Serial.println("\n=== DONE ===");
}

void loop() {
  delay(1000);
}

