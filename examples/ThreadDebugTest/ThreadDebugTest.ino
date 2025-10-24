#include "Arduino.h"
#include "mbed.h"
#include "Nicla_System.h"

// Debug thread system step by step
static rtos::Queue<String, 10> debug_queue;
static rtos::Thread debug_thread(osPriorityNormal, 1024, nullptr, "debug_thread");
static bool debug_thread_running = false;
static int debug_counter = 0;

void debug_thread_function() {
    Serial.println("DEBUG: Thread started");
    while (debug_thread_running) {
        String* debug_msg;
        if (debug_queue.try_get(&debug_msg)) {
            Serial.print("DEBUG_MSG: ");
            Serial.print(*debug_msg);
            delete debug_msg;
            debug_counter++;
        }
        rtos::ThisThread::sleep_for(10);
    }
    Serial.println("DEBUG: Thread stopped");
}

void start_debug_thread() {
    Serial.println("DEBUG: Starting thread...");
    if (!debug_thread_running) {
        debug_thread_running = true;
        debug_thread.start(debug_thread_function);
        Serial.println("DEBUG: Thread start() called");
    } else {
        Serial.println("DEBUG: Thread already running");
    }
}

void stop_debug_thread() {
    Serial.println("DEBUG: Stopping thread...");
    debug_thread_running = false;
    debug_thread.join();
    Serial.println("DEBUG: Thread joined");
}

void setup() {
  Serial.begin(1000000);  // 1 Mbps
  while (!Serial) delay(10);
  
  Serial.println("=== THREAD DEBUG TEST START ===");
  Serial.println("Arduino Nicla Voice - Thread Debug Test");
  Serial.println("Timestamp: " + String(millis()));
  
  // Initialize Nicla board components
  nicla::begin();
  nicla::disableLDO();
  nicla::leds.begin();

  Serial.println("GPIO: OK");

  // Test thread system
  Serial.println("THREAD:TEST_START");
  
  // Start debug thread
  start_debug_thread();
  delay(100);
  
  // Send test messages
  Serial.println("THREAD:SENDING_MESSAGES");
  for (int i = 0; i < 5; i++) {
    String* msg = new String("Test message ");
    *msg += i;
    *msg += "\n";
    
    Serial.print("THREAD:PUT_MSG_");
    Serial.print(i);
    Serial.print(": ");
    
    bool result = debug_queue.try_put(msg);
    Serial.println(result ? "SUCCESS" : "FAILED");
    
    delay(100);
  }
  
  // Wait for messages to be processed
  Serial.println("THREAD:WAITING_FOR_PROCESSING");
  delay(1000);
  
  // Stop thread
  stop_debug_thread();
  
  Serial.print("THREAD:PROCESSED_COUNT=");
  Serial.println(debug_counter);

  Serial.println("=== THREAD DEBUG TEST SETUP COMPLETE ===");
}

void loop() {
  Serial.println("LOOP: " + String(millis()));
  delay(1000);
}
