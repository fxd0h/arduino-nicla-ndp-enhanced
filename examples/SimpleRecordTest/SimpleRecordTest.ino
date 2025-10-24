/*
  Simple Record Test - Based on Arduino Record and Stream
  Removes streaming/encoding, adds simple counter
  Shows bytes captured every 1 second
*/

#include "Arduino.h"
#include "NDP.h"

uint8_t data[2048];
int chunk_size = 0;
int audio_extractions = 0;
unsigned long start_time;
unsigned long last_audio_time;
unsigned long last_stats_time;
bool setup_complete = false;

void ledGreenOn() {
  nicla::leds.begin();
  nicla::leds.setColor(green);
  delay(200);
  nicla::leds.setColor(off);
  nicla::leds.end();
}

void setup() {
  Serial.begin(1000000);  // 1 Mbps for faster logging
  while (!Serial) delay(10);
  
  Serial.println("=== SIMPLE RECORD TEST START ===");
  Serial.println("Arduino Nicla Voice - Simple Record Test");
  Serial.println("Based on Record_and_stream.ino");
  Serial.println("Timestamp: " + String(millis()));
  
  // Initialize Nicla
  Serial.println("NICLA:INIT");
  nicla::begin();
  nicla::disableLDO();
  nicla::leds.begin();
  Serial.println("NICLA:OK");
  
  // Set up NDP event handler
  Serial.println("NDP:EVENT_HANDLER");
  NDP.onEvent(ledGreenOn);
  Serial.println("NDP:EVENT_HANDLER_OK");
  
  // Load firmwares (exact same as original)
  Serial.println("MCU:LOAD_START");
  NDP.begin("mcu_fw_120_v91.synpkg");
  Serial.println("MCU:LOAD_DONE");
  
  Serial.println("DSP:LOAD_START");
  NDP.load("dsp_firmware_v91.synpkg");
  Serial.println("DSP:LOAD_DONE");
  
  Serial.println("NN:LOAD_START");
  NDP.load("alexa_334_NDP120_B0_v11_v91.synpkg");
  Serial.println("NN:LOAD_DONE");
  
  // Turn on microphone
  Serial.println("MIC:START");
  NDP.turnOnMicrophone();
  Serial.println("MIC:OK");
  
  // Get audio chunk size
  Serial.println("CHUNK:SIZE_CHECK");
  chunk_size = NDP.getAudioChunkSize();
  Serial.println("CHUNK:SIZE=" + String(chunk_size));
  
  if (chunk_size >= sizeof(data)) {
    Serial.println("ERROR:CHUNK_SIZE_TOO_LARGE");
    for(;;);
  }
  
  Serial.println("SETUP:COMPLETE");
  Serial.println("=== STARTING AUDIO EXTRACTION LOOP ===");
  
  start_time = millis();
  last_audio_time = start_time;
  last_stats_time = start_time;
  setup_complete = true;
}

void loop() {
  if (!setup_complete) return;
  
  unsigned int len = 0;
  
  // Extract audio data (exact same as original)
  NDP.extractData(data, &len);
  
  if (len > 0) {
    audio_extractions++;
    last_audio_time = millis();
  }
  
  // Display stats every 1 second
  unsigned long current_time = millis();
  if (current_time - last_stats_time >= 1000) {
    Serial.println("STATS:EXTRACTIONS=" + String(audio_extractions) + 
                   " BYTES=" + String(audio_extractions * chunk_size) + 
                   " TIME=" + String(current_time - start_time));
    last_stats_time = current_time;
  }
  
  // Check for timeout (no audio for 10 seconds)
  if (millis() - last_audio_time > 10000) {
    Serial.println("TIMEOUT:NO_AUDIO");
    Serial.println("FINAL:EXTRACTIONS=" + String(audio_extractions));
    Serial.println("FINAL:BYTES=" + String(audio_extractions * chunk_size));
    Serial.println("FINAL:UPTIME=" + String(millis() - start_time));
    Serial.println("=== SIMPLE RECORD TEST COMPLETE ===");
    
    // Stop and wait
    for(;;);
  }
}