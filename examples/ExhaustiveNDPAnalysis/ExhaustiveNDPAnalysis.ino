/**
 * @file ExhaustiveNDPAnalysis.ino
 * @brief Análisis exhaustivo de NDP120 - Arduino vs Zephyr
 * 
 * Registra TODO lo que hace Arduino con NDP120:
 * - Inicialización SPI
 * - Valores enviados/recibidos
 * - Timeouts y esperas
 * - Mailbox flow
 * - Timestamps crípticos
 * 
 * @author AI Agent
 * @date 2025-01-23
 */

#include <NDP.h>

// Timestamp tracking
unsigned long start_time;
unsigned long last_timestamp;

// Helper para timestamp críptico
void log_timestamp(const char* prefix) {
  unsigned long now = millis();
  unsigned long delta = now - last_timestamp;
  last_timestamp = now;
  
  Serial.print("[");
  Serial.print(now - start_time);
  Serial.print("+");
  Serial.print(delta);
  Serial.print("] ");
  Serial.print(prefix);
  Serial.println();
}

// Helper para valores hex crípticos
void log_hex(const char* prefix, uint8_t* data, size_t len) {
  Serial.print(prefix);
  for (size_t i = 0; i < len; i++) {
    if (data[i] < 16) Serial.print("0");
    Serial.print(data[i], HEX);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  start_time = millis();
  last_timestamp = start_time;
  
  log_timestamp("=== EXHAUSTIVE NDP120 ANALYSIS ===");
  log_timestamp("INIT:START");
  
  // === 1. SPI INITIALIZATION ===
  log_timestamp("SPI:INIT");
  
  // Log SPI configuration
  Serial.println("SPI:CFG");
  Serial.print("  MOSI="); Serial.println(MOSI);
  Serial.print("  MISO="); Serial.println(MISO);
  Serial.print("  SCK="); Serial.println(SCK);
  Serial.print("  CS=31");
  Serial.print("  INT=30");
  Serial.print("  RST=18");
  
  // Initialize SPI - Arduino mbed style
  SPI1.begin();
  SPI1.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  
  log_timestamp("SPI:CFG_DONE");
  
  // === 2. GPIO SETUP ===
  log_timestamp("GPIO:INIT");
  
  pinMode(31, OUTPUT);  // CS pin
  pinMode(30, INPUT);   // INT pin
  pinMode(18, OUTPUT);  // RST pin
  
  digitalWrite(31, HIGH);  // CS HIGH
  digitalWrite(18, HIGH);  // RST HIGH
  
  log_timestamp("GPIO:SET");
  
  // === 3. NDP120 RESET SEQUENCE ===
  log_timestamp("RST:START");
  
  // Reset sequence
  digitalWrite(18, LOW);   // RST LOW
  log_timestamp("RST:LOW");
  delay(100); // 100ms LOW
  
  digitalWrite(18, HIGH);  // RST HIGH
  log_timestamp("RST:HIGH");
  delay(100); // 100ms HIGH
  
  log_timestamp("RST:DONE");
  
  // === 4. BASIC SPI TEST ===
  log_timestamp("SPI:TEST");
  
  // Test basic SPI communication
  digitalWrite(31, LOW);   // CS LOW
  uint8_t test_cmd = 0x80; // Read command
  uint8_t test_response = SPI1.transfer(test_cmd);
  digitalWrite(31, HIGH);  // CS HIGH
  
  Serial.print("SPI:TEST_CMD=0x");
  Serial.println(test_cmd, HEX);
  Serial.print("SPI:TEST_RSP=0x");
  Serial.println(test_response, HEX);
  
  log_timestamp("SPI:TEST_DONE");
  
  // === 5. CHIP ID TEST ===
  log_timestamp("ID:START");
  
  uint8_t chip_id[4];
  for (int i = 0; i < 4; i++) {
    digitalWrite(31, LOW);  // CS LOW
    uint8_t cmd = 0x80 | i;
    chip_id[i] = SPI1.transfer(cmd);
    digitalWrite(31, HIGH);  // CS HIGH
    
    Serial.print("ID:REG");
    Serial.print(i);
    Serial.print("=0x");
    if (chip_id[i] < 16) Serial.print("0");
    Serial.println(chip_id[i], HEX);
    
    delay(1); // Small delay between reads
  }
  
  log_timestamp("ID:DONE");
  
  // === 6. NDP INITIALIZATION ===
  log_timestamp("NDP:INIT");
  
  int init_result = NDP.begin("mcu_fw_120_v91.synpkg");
  Serial.print("NDP:INIT_RES=");
  Serial.println(init_result);
  
  log_timestamp("NDP:INIT_DONE");
  
  // === 7. FIRMWARE LOADING ===
  log_timestamp("FW:START");
  
  // Load MCU firmware
  log_timestamp("FW:MCU_START");
  int mcu_result = NDP.load("mcu_fw_120_v91.synpkg");
  Serial.print("FW:MCU_RES=");
  Serial.println(mcu_result);
  log_timestamp("FW:MCU_DONE");
  
  // Load DSP firmware
  log_timestamp("FW:DSP_START");
  int dsp_result = NDP.load("dsp_firmware_v91.synpkg");
  Serial.print("FW:DSP_RES=");
  Serial.println(dsp_result);
  log_timestamp("FW:DSP_DONE");
  
  // Load NN model
  log_timestamp("FW:NN_START");
  int nn_result = NDP.load("alexa_334_NDP120_B0_v11_v91.synpkg");
  Serial.print("FW:NN_RES=");
  Serial.println(nn_result);
  log_timestamp("FW:NN_DONE");
  
  log_timestamp("FW:ALL_DONE");
  
  // === 8. CLOCK CONFIGURATION ===
  log_timestamp("CLK:START");
  
  int clk_result = NDP.turnOnMicrophone();
  Serial.print("CLK:RES=");
  Serial.println(clk_result);
  
  log_timestamp("CLK:DONE");
  
  // === 9. AUDIO CHUNK SIZE TEST ===
  log_timestamp("AUD:START");
  
  uint32_t chunk_size = NDP.getAudioChunkSize();
  Serial.print("AUD:CHUNK=");
  Serial.println(chunk_size);
  
  log_timestamp("AUD:DONE");
  
  // === 10. MAILBOX ANALYSIS ===
  log_timestamp("MB:START");
  
  // Read mailbox registers
  for (int i = 0; i < 8; i++) {
    digitalWrite(31, LOW);  // CS LOW
    uint8_t cmd = 0x80 | (0x04 + i); // MBOUT + other registers
    uint8_t value = SPI1.transfer(cmd);
    digitalWrite(31, HIGH);  // CS HIGH
    
    Serial.print("MB:REG");
    Serial.print(0x04 + i, HEX);
    Serial.print("=0x");
    if (value < 16) Serial.print("0");
    Serial.println(value, HEX);
    
    delay(1);
  }
  
  log_timestamp("MB:DONE");
  
  // === 11. SPI SPEED TEST ===
  log_timestamp("SPI:SPEED_TEST");
  
  // Test different speeds
  int speeds[] = {1000000, 2000000, 4000000, 8000000};
  const char* speed_names[] = {"1MHz", "2MHz", "4MHz", "8MHz"};
  
  for (int i = 0; i < 4; i++) {
    SPI1.beginTransaction(SPISettings(speeds[i], MSBFIRST, SPI_MODE0));
    log_timestamp(("SPI:" + String(speed_names[i])).c_str());
    
    // Test read at this speed
    digitalWrite(31, LOW);  // CS pin
    uint8_t test_resp = SPI1.transfer(0x80);
    digitalWrite(31, HIGH);
    
    Serial.print("SPI:TEST_");
    Serial.print(speed_names[i]);
    Serial.print("=0x");
    if (test_resp < 16) Serial.print("0");
    Serial.println(test_resp, HEX);
    
    delay(10);
  }
  
  log_timestamp("SPI:SPEED_DONE");
  
  // === 12. CONTINUOUS MONITORING ===
  log_timestamp("MON:START");
  
  for (int i = 0; i < 10; i++) {
    log_timestamp("MON:LOOP");
    
    // Read key registers
    digitalWrite(31, LOW);  // CS LOW
    uint8_t mbout = SPI1.transfer(0x84); // MBOUT
    digitalWrite(31, HIGH);  // CS HIGH
    
    Serial.print("MON:MB=");
    if (mbout < 16) Serial.print("0");
    Serial.println(mbout, HEX);
    
    // Check audio chunk size
    uint32_t chunk = NDP.getAudioChunkSize();
    Serial.print("MON:CHUNK=");
    Serial.println(chunk);
    
    delay(100);
  }
  
  log_timestamp("MON:DONE");
  
  log_timestamp("=== ANALYSIS COMPLETE ===");
}

void loop() {
  // Continuous monitoring
  static unsigned long last_loop = 0;
  unsigned long now = millis();
  
  if (now - last_loop >= 1000) { // Every 1 second
    last_loop = now;
    
    log_timestamp("LOOP:1S");
    
    // Read MBOUT
    digitalWrite(31, LOW);  // CS LOW
    uint8_t mbout = SPI1.transfer(0x84);
    digitalWrite(31, HIGH);  // CS HIGH
    
    Serial.print("LOOP:MB=");
    if (mbout < 16) Serial.print("0");
    Serial.println(mbout, HEX);
    
    // Check audio chunk size
    uint32_t chunk = NDP.getAudioChunkSize();
    Serial.print("LOOP:CHUNK=");
    Serial.println(chunk);
  }
}
