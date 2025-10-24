/*
  Record and Stream Analysis - Based on Arduino NDP Record_and_stream example
  This sketch performs the complete NDP120 workflow without streaming to serial
  Includes exhaustive logging for analysis
*/

#include "Arduino.h"
#include "NDP.h"

// Exhaustive logging macros (already implemented in NDP.cpp)
#define LOG_NDP(msg) Serial.print("[NDP:"); Serial.print(millis()); Serial.print("] "); Serial.println(msg)
#define LOG_NDP_HEX(msg, val) Serial.print("[NDP:"); Serial.print(millis()); Serial.print("] "); Serial.print(msg); Serial.print("=0x"); if(val<16)Serial.print("0"); Serial.println(val, HEX)
#define LOG_NDP_DEC(msg, val) Serial.print("[NDP:"); Serial.print(millis()); Serial.print("] "); Serial.print(msg); Serial.print("="); Serial.println(val)
#define LOG_NDP_BUF(msg, buf, len) Serial.print("[NDP:"); Serial.print(millis()); Serial.print("] "); Serial.print(msg); for(int i=0; i<len; i++) { if(((uint8_t*)buf)[i]<16)Serial.print("0"); Serial.print(((uint8_t*)buf)[i], HEX); } Serial.println()

uint8_t data[2048];
int chunk_size = 0;
int audio_extractions = 0;
unsigned long start_time;
unsigned long last_audio_time;

void ledGreenOn() {
  LOG_NDP("LED:GREEN");
  nicla::leds.begin();
  nicla::leds.setColor(green);
  delay(200);
  nicla::leds.setColor(off);
  nicla::leds.end();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  LOG_NDP("=== RECORD AND STREAM ANALYSIS START ===");
  LOG_NDP("Based on Arduino NDP Record_and_stream example");
  LOG_NDP("Timestamp: " + String(millis()));
  
  // Initialize Nicla
  LOG_NDP("NICLA:INIT");
  nicla::begin();
  nicla::disableLDO();
  nicla::leds.begin();
  LOG_NDP("NICLA:OK");
  
  // Set up NDP event handler
  LOG_NDP("NDP:EVENT_HANDLER");
  NDP.onEvent(ledGreenOn);
  
  // Load MCU firmware
  LOG_NDP("MCU:LOAD_START");
  NDP.begin("mcu_fw_120_v91.synpkg");
  LOG_NDP("MCU:LOAD_DONE");
  
  // Load DSP firmware
  LOG_NDP("DSP:LOAD_START");
  NDP.load("dsp_firmware_v91.synpkg");
  LOG_NDP("DSP:LOAD_DONE");
  
  // Load NN Model
  LOG_NDP("NN:LOAD_START");
  NDP.load("alexa_334_NDP120_B0_v11_v91.synpkg");
  LOG_NDP("NN:LOAD_DONE");
  
  // Turn on microphone
  LOG_NDP("MIC:START");
  NDP.turnOnMicrophone();
  LOG_NDP("MIC:OK");
  
  // Get audio chunk size
  LOG_NDP("CHUNK:SIZE_CHECK");
  chunk_size = NDP.getAudioChunkSize();
  LOG_NDP_DEC("CHUNK:SIZE", chunk_size);
  
  if (chunk_size >= sizeof(data)) {
    LOG_NDP("ERROR:CHUNK_SIZE_TOO_LARGE");
    for(;;);
  }
  
  LOG_NDP("SETUP:COMPLETE");
  LOG_NDP("=== STARTING AUDIO EXTRACTION LOOP ===");
  
  start_time = millis();
  last_audio_time = start_time;
}

void loop() {
  unsigned int len = 0;
  
  // Extract audio data
  NDP.extractData(data, &len);
  
  if (len > 0) {
    audio_extractions++;
    last_audio_time = millis();
    
    // Log audio extraction details
    LOG_NDP_DEC("AUDIO:EXTRACT", audio_extractions);
    LOG_NDP_DEC("AUDIO:LEN", len);
    LOG_NDP_DEC("AUDIO:TIME", last_audio_time - start_time);
    
    // Log first few bytes of audio data
    if (len > 0) {
      LOG_NDP_BUF("AUDIO:DATA", data, min(len, 16));
    }
    
    // Log statistics every 100 extractions
    if (audio_extractions % 100 == 0) {
      LOG_NDP_DEC("STATS:EXTRACTIONS", audio_extractions);
      LOG_NDP_DEC("STATS:UPTIME", last_audio_time - start_time);
      LOG_NDP_DEC("STATS:AVG_INTERVAL", (last_audio_time - start_time) / audio_extractions);
    }
  }
  
  // Check for timeout (no audio for 5 seconds)
  if (millis() - last_audio_time > 5000) {
    LOG_NDP("TIMEOUT:NO_AUDIO");
    LOG_NDP_DEC("FINAL:EXTRACTIONS", audio_extractions);
    LOG_NDP_DEC("FINAL:UPTIME", millis() - start_time);
    LOG_NDP("=== ANALYSIS COMPLETE ===");
    
    // Stop and wait
    for(;;);
  }
}
