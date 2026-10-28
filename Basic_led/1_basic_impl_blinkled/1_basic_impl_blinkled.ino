// === STEP 1: Push Button (INPUT_PULLUP) + RGB LED (Common Anode) + Non-blocking millis() ===
// ผังขา Master Pinout:
// - RGB LED: D9 (Red), D10 (Green), D11 (Blue), ขายาวต่อ +5V (Common Anode)
// - Push Button: ขาหนึ่งต่อ A2, อีกขาต่อ GND (ใช้ INPUT_PULLUP)

const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;
const int buttonPin = A2;

const bool IS_COMMON_ANODE = true;

// ตัวแปรสำหรับจับเวลาด้วย millis() (ห้ามใช้ delay() ตอนสอบ MQTT!)
unsigned long lastChangeTime = 0;
int testState = 0;  // 0 = ทดสอบช่วง 36-50 (แดง), 1 = ทดสอบช่วง 26-35 (ฟ้า/น้ำเงิน), 2 = ทดสอบช่วง 10-25 (เขียว)

void setColor(int red, int green, int blue) {
  if (IS_COMMON_ANODE) {
    analogWrite(redPin, 255 - red);
    analogWrite(greenPin, 255 - green);
    analogWrite(bluePin, 255 - blue);
  } else {
    analogWrite(redPin, red);
    analogWrite(greenPin, green);
    analogWrite(bluePin, blue);
  }
}

// ฟังก์ชันจำลองโจทย์ข้อ Subscribe 1: อุณหภูมิดาวศุกร์ (venus)
void showVenusTempColor(int temp) {
  if (temp >= 36 && temp <= 50) {
    setColor(255, 0, 0);  // 36 - 50 -> สีแดง
  } else if (temp >= 26 && temp <= 35) {
    setColor(0, 0, 255);  // 26 - 35 -> สีฟ้า/น้ำเงิน
  } else if (temp >= 10 && temp <= 25) {
    setColor(0, 255, 0);  // 10 - 25 -> สีเขียว
  } else {
    setColor(0, 0, 0);  // นอกช่วง -> ดับไฟ
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // ใช้ INPUT_PULLUP: ไม่กด = HIGH, กดปุ่ม = LOW
  pinMode(buttonPin, INPUT_PULLUP);

  setColor(0, 0, 0);
  Serial.println("=== Step 1: Button + RGB LED Ready ===");
}

void loop() {
  // อ่านค่าปุ่มกด (INPUT_PULLUP -> กดคือ LOW)
  bool isPressed = (digitalRead(buttonPin) == LOW);

  if (isPressed) {
    // เมื่อกดปุ่ม: ให้ไฟติดสีขาว (R+G+B) ทันทีโดยไม่ดีเลย์!
    setColor(255, 255, 255);
    Serial.println("Button PRESSED (LOW) -> White LED!");
    delay(50);  // debounce เล็กน้อย
  } else {
    // เมื่อไม่ได้กดปุ่ม: สลับจำลองอุณหภูมิดาวศุกร์ทุกๆ 1.5 วินาทีด้วย millis()
    if (millis() - lastChangeTime >= 1500) {
      lastChangeTime = millis();

      int simulatedTemp = 0;
      if (testState == 0) simulatedTemp = 43;       // ช่วง 36-50 -> แดง
      else if (testState == 1) simulatedTemp = 30;  // ช่วง 26-35 -> ฟ้า/น้ำเงิน
      else if (testState == 2) simulatedTemp = 18;  // ช่วง 10-25 -> เขียว

      Serial.print("Simulated Venus Temp: ");
      Serial.println(simulatedTemp);
      showVenusTempColor(simulatedTemp);

      testState = (testState + 1) % 3;
    }
  }
}
