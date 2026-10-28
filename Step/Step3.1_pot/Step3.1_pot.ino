
const int potPin = A0;
void setup() {
  Serial.begin(9600);
}
void loop() {
  int sensorValue = analogRead(potPin);
  
  Serial.println(analogRead(potPin));
  
  // Short delay for stability:
  delay(1);
}