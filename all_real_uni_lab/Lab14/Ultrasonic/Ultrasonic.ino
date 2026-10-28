// ถ้าเสียบตาม Lab14 เดิมใช้ Trig=4, Echo=2 (ถ้าเสียบตามโพยรวมให้แก้เป็น Trig=12, Echo=13)
const int trigPin = 4;
const int echoPin = 2;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // 1. เคลียร์ขา Trig ให้เป็น LOW ก่อน 2 ไมโครวินาที
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // 2. ยิงคลื่นเสียงโดยสั่ง HIGH ค้างไว้ 10 ไมโครวินาที แล้วปิดเป็น LOW
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // 3. อ่านเวลาที่คลื่นสะท้อนกลับมาที่ขา Echo (ใส่ timeout 30000 us กันค้าง)
  long pulseWidth = pulseIn(echoPin, HIGH, 30000);
  long distance = pulseWidth / 29 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(500);
}
