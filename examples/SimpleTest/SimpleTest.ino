void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  Serial.println("=== SIMPLE TEST START ===");
  Serial.println("Arduino Nicla Voice - Basic Test");
  Serial.println("Timestamp: " + String(millis()));
  
  // Test basic GPIO
  pinMode(31, OUTPUT);  // CS
  pinMode(18, OUTPUT);  // RST
  pinMode(30, INPUT);    // INT
  
  digitalWrite(31, HIGH);
  digitalWrite(18, HIGH);
  
  Serial.println("GPIO: OK");
  Serial.println("=== SIMPLE TEST DONE ===");
}

void loop() {
  Serial.println("LOOP: " + String(millis()));
  delay(1000);
}

