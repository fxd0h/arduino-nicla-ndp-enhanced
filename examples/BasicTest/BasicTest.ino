void setup() {
  Serial.begin(115200);
  delay(1000);  // Wait for serial to initialize
  
  Serial.println("=== BASIC TEST START ===");
  Serial.println("Arduino Nicla Voice - Basic Test");
  Serial.println("Timestamp: " + String(millis()));
  
  // Test basic GPIO
  pinMode(31, OUTPUT);  // CS
  pinMode(18, OUTPUT);  // RST
  pinMode(30, INPUT);    // INT
  
  digitalWrite(31, HIGH);
  digitalWrite(18, HIGH);
  
  Serial.println("GPIO: OK");
  Serial.println("=== BASIC TEST DONE ===");
}

void loop() {
  Serial.println("LOOP: " + String(millis()));
  delay(1000);
}
