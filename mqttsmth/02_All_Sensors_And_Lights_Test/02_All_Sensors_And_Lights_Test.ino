// ไฟล์ทดสอบอุปกรณ์ทุกชิ้นบนบอร์ดเดียว (แบบยังไม่ต่อ WiFi/MQTT) เอาไว้เช็คสายไฟในห้องสอบ
#include <math.h>

const int segPins[7] = {2, 3, 4, 5, 6, 7, 8}; // a, b, c, d, e, f, g
const int redPin     = 9;
const int greenPin   = 10;
const int bluePin    = 11;
const int trigPin    = 12;
const int echoPin    = 13;
const int tempPin    = A0;
const int lightPin   = A1;
const int buttonPin  = A2;

const int num_array[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};

int testStep = 0;

void setColor(int r, int g, int b) {
  analogWrite(redPin,   255 - r); // Common Anode
  analogWrite(greenPin, 255 - g);
  analogWrite(bluePin,  255 - b);
}

void displayDigit(int digit) {
  if (digit < 0 || digit > 9) return;
  for (int i = 0; i < 7; i++) {
    digitalWrite(segPins[i], num_array[digit][i]);
  }
}

long readDistanceCm() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long pulseWidth = pulseIn(echoPin, HIGH, 30000);
  return pulseWidth / 29 / 2;
}

float readTemperatureC() {
  int raw = analogRead(tempPin);
  float voltage = raw * (5.0 / 1024.0);
  return (voltage - 0.5) * 100.0;
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) pinMode(segPins[i], OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  // 1. ทดสอบแสดงเลข 0-9 บน 7-Segment และสลับสี RGB
  displayDigit(testStep % 10);
  if (testStep % 3 == 0)      setColor(255, 0, 0); // แดง
  else if (testStep % 3 == 1) setColor(0, 255, 0); // เขียว
  else                        setColor(0, 0, 255); // น้ำเงิน
  testStep++;

  // 2. ทดสอบอ่านค่าเซ็นเซอร์ทั้งหมดออก Serial Monitor
  float tempC = readTemperatureC();
  int lightVal = analogRead(lightPin);
  long dist = readDistanceCm();
  bool pressed = (digitalRead(buttonPin) == LOW);

  Serial.print("Temp: "); Serial.print(tempC, 1);
  Serial.print(" C | Light: "); Serial.print(lightVal);
  Serial.print(" | Dist: "); Serial.print(dist);
  Serial.print(" cm | Button: "); Serial.println(pressed ? "PRESSED" : "RELEASED");

  delay(500);
}
