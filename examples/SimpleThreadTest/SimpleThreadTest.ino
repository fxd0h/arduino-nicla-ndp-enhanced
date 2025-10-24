#include "Arduino.h"
#include "mbed.h"
#include "Nicla_System.h"

// Simple thread test without NDP
static rtos::Queue<String, 20> test_queue;
static rtos::Thread test_thread(osPriorityNormal, 1024, nullptr, "test_thread");
static bool test_thread_running = false;

void test_thread_function() {
    while (test_thread_running) {
        String* test_msg;
        if (test_queue.try_get(&test_msg)) {
            Serial.print(*test_msg);
            delete test_msg;
        }
        rtos::ThisThread::sleep_for(10);
    }
}

void start_test_thread() {
    if (!test_thread_running) {
        test_thread_running = true;
        test_thread.start(test_thread_function);
    }
}

void stop_test_thread() {
    test_thread_running = false;
    test_thread.join();
}

void setup() {
  Serial.begin(1000000);  // 1 Mbps
  while (!Serial) delay(10);
  
  Serial.println("=== SIMPLE THREAD TEST START ===");
  Serial.println("Arduino Nicla Voice - Simple Thread Test");
  Serial.println("Timestamp: " + String(millis()));
  
  // Initialize Nicla board components
  nicla::begin();
  nicla::disableLDO();
  nicla::leds.begin();

  Serial.println("GPIO: OK");

  // Start test thread
  start_test_thread();
  Serial.println("THREAD:STARTED");

  // Send some test messages to the queue
  for (int i = 0; i < 10; i++) {
    String* msg = new String("THREAD_MSG:");
    *msg += i;
    *msg += "\n";
    test_queue.try_put(msg);
    delay(100);
  }

  Serial.println("=== SIMPLE THREAD TEST SETUP COMPLETE ===");
}

void loop() {
  Serial.println("LOOP: " + String(millis()));
  delay(1000);
}
