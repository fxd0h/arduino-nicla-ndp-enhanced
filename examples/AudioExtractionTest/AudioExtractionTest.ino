/*
  Audio Extraction Test - Minimal sketch without logs
  Tests if sketch executes without logging interference
*/

#include "Arduino.h"
#include "NDP.h"

uint8_t data[2048];
int chunk_size = 0;
int audio_extractions = 0;
unsigned long start_time;
unsigned long last_audio_time;
bool setup_complete = false;

void setup() {
  Serial.begin(1000000);
  while (!Serial) delay(10);
  
  Serial.println("SETUP:START");
  
  // Initialize Nicla
  nicla::begin();
  nicla::disableLDO();
  nicla::leds.begin();
  
  // Set up NDP event handler
  NDP.onEvent([]() {
    nicla::leds.begin();
    nicla::leds.setColor(green);
    delay(200);
    nicla::leds.setColor(off);
    nicla::leds.end();
  });
  
  // Load firmwares
  NDP.begin("mcu_fw_120_v91.synpkg");
  NDP.load("dsp_firmware_v91.synpkg");
  NDP.load("alexa_334_NDP120_B0_v11_v91.synpkg");
  
  // Turn on microphone
  NDP.turnOnMicrophone();
  
  // Get audio chunk size
  chunk_size = NDP.getAudioChunkSize();
  
  if (chunk_size >= sizeof(data)) {
    Serial.println("ERROR:CHUNK_SIZE_TOO_LARGE");
    for(;;);
  }
  
  Serial.println("SETUP:COMPLETE");
  
  start_time = millis();
  last_audio_time = start_time;
  setup_complete = true;
}

void loop() {
  if (!setup_complete) return;
  
  unsigned int len = 0;
  
  // Extract audio data
  NDP.extractData(data, &len);
  
  if (len > 0) {
    audio_extractions++;
    last_audio_time = millis();
    
    // Minimal logging
    Serial.println("AUDIO:" + String(audio_extractions) + ":" + String(len));
    
    // Log statistics every 10 extractions
    if (audio_extractions % 10 == 0) {
      Serial.println("STATS:" + String(audio_extractions) + ":" + String(last_audio_time - start_time));
    }
  }
  
  // Check for timeout (no audio for 10 seconds)
  if (millis() - last_audio_time > 10000) {
    Serial.println("TIMEOUT:NO_AUDIO");
    Serial.println("FINAL:" + String(audio_extractions) + ":" + String(millis() - start_time));
    
    // Stop and wait
    for(;;);
  }
  
  // Small delay to prevent overwhelming
  delay(10);
}
