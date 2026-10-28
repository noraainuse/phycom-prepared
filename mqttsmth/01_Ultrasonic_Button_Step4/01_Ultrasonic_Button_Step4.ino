// Step 4: ทดสอบ Ultrasonic (HC-SR04) + ปุ่มกด (Push Button) สำหรับข้อ food และ door
// หมายเหตุ: ถ้าเสียบสายตาม Lab14 เดิม ให้เปลี่ยน trigPin=4, echoPin=2
const int trigPin   = 12; // หรือเปลี่ยนเป็น 4
const int echoPin   = 13; // หรือเปลี่ยนเป็น 2
const int buttonPin = A2; // ต่อปุ่มกดระหว่าง A2 กับ GND

long readDistanceCm() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long pulseWidth = pulseIn(echoPin, HIGH, 30000); // timeout 30ms
  return pulseWidth / 29 / 2;
}

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  long distance = readDistanceCm();
  bool isPressed = (digitalRead(buttonPin) == LOW);

  // ข้อ 68070141/food: > 20 cm ส่ง "off", <= 20 cm ส่ง "on" (พิมพ์เล็ก)
  String foodMsg = (distance > 20) ? "off" : "on";

  // ข้อ 68070141/door: >= 20 cm และกดปุ่ม ส่ง "On", กรณีอื่นส่ง "Off" (O พิมพ์ใหญ่)
  String doorMsg = (distance >= 20 && isPressed) ? "On" : "Off";

  Serial.print("Distance: "); Serial.print(distance);
  Serial.print(" cm | Button: "); Serial.print(isPressed ? "PRESSED" : "RELEASED");
  Serial.print(" | food: "); Serial.print(foodMsg);
  Serial.print(" | door: "); Serial.println(doorMsg);

  delay(500);
}
