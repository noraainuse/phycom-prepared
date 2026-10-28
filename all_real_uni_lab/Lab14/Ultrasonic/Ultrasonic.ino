void setup() {
  Serial.begin(9600);
  pinMode(4, OUTPUT);  // 4
  pinMode(2, INPUT);
}

void loop() {
  digitalWrite(4, HIGH);
  delayMicroseconds(10);
  digitalWrite(4, LOW);

  int pulseWidth = pulseIn(2, HIGH);

  digitalWrite(4, HIGH);
  delayMicroseconds(10);  // เปิดสัญญาณค้างไว้ 10 ไมโครวินาที (สั้นมากๆ) เพื่อกระตุ้นเซ็นเซอร์
  digitalWrite(4, LOW);   // ปิดสัญญาณ -> ทันทีที่ปิด เซ็นเซอร์จะยิงคลื่นเสียงความถี่สูงออกไป
  long distance = pulseWidth / 29 / 2;
  Serial.print("Distance: ");
  Serial.println(distance);  // พิมพ์ค่าระยะทาง (cm) ออกทางหน้าจอ Serial Monitor
  delay(1000);
}
