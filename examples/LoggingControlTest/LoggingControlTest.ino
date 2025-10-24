/*
  Logging Control Test - Tests new NDP logging control methods
  Demonstrates how to disable NDP logging by default
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
  Serial.begin(1000000);
  while (!Serial) delay(10);
  
  Serial.println("LOGGING_CONTROL_TEST:START");
  
  // Initialize Nicla
  nicla::begin();
  nicla::disableLDO();
  nicla::leds.begin();
  
  // Set up NDP event handler
  NDP.onEvent(ledGreenOn);
  
  // Test logging control methods
  Serial.println("LOGGING:TESTING_CONTROL_METHODS");
  Serial.println("LOGGING:ENABLED=" + String(NDP.isLoggingEnabled()));
  
  // Disable logging before loading firmwares
  Serial.println("LOGGING:DISABLING");
  NDP.disableLogging();
  Serial.println("LOGGING:ENABLED=" + String(NDP.isLoggingEnabled()));
  
  // Load firmwares with logging disabled
  Serial.println("MCU:LOAD_START");
  NDP.begin("mcu_fw_120_v91.synpkg", false);  // Explicitly disable logging
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
  
  Serial.println("LOGGING_CONTROL_TEST:SETUP_COMPLETE");
  Serial.println("LOGGING:ENABLED=" + String(NDP.isLoggingEnabled()));
  
  start_time = millis();
  last_audio_time = start_time;
  last_stats_time = start_time;
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
  }
  
  // Display stats every 1 second
  unsigned long current_time = millis();
  if (current_time - last_stats_time >= 1000) {
    Serial.println("STATS:" + String(audio_extractions) + ":" + String(audio_extractions * chunk_size));
    last_stats_time = current_time;
  }
  
  // Check for timeout (no audio for 10 seconds)
  if (millis() - last_audio_time > 10000) {
    Serial.println("TIMEOUT:NO_AUDIO");
    Serial.println("FINAL:" + String(audio_extractions) + ":" + String(audio_extractions * chunk_size));
    
    // Stop and wait
    for(;;);
  }
}
