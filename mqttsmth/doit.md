# คู่มือทำสอบฉบับจับมือทำ (DO IT ALL — เซ็นเซอร์ + ไฟ/จอทุกรูปแบบ + MQTT Version)

เอกสารนี้รวม **วิธีดูขา + วิธีเสียบสายทีละเส้น + โค้ดแยกเดี่ยว + โค้ดเวอร์ชัน MQTT** ของอุปกรณ์ทุกตัวที่มีโอกาสออกสอบ Physical Computing (Arduino UNO R4 WiFi) เปิดค้นหา (`Ctrl + F`) แล้วก๊อปปี้ไปใช้ในห้องสอบได้ทันที

---

## สารบัญด่วน (กดข้ามไปหัวข้อที่ต้องการ)
1. [ผังจัดสรรขา Master Pinout (ต่อทุกอย่างพร้อมกันบนบอร์ดเดียวไม่ชนกัน)](#1-ผังจัดสรรขา-master-pinout)
2. [หมวดไฟ & จอแสดงผล (Outputs — มักออกฝั่ง Subscribe หรือแสดงผลตามเซ็นเซอร์)](#2-หมวดไฟ--จอแสดงผล-outputs)
   * [2.1 หลอดไฟ LED เดี่ยว (เปิด/ปิด Digital & หรี่ไฟ PWM)](#21-หลอดไฟ-led-เดี่ยว-เปิดปิด--หรี่ไฟ-pwm)
   * [2.2 หลอดไฟ RGB LED 4 ขา (ทั้งแบบ Common Anode และ Common Cathode)](#22-หลอดไฟ-rgb-led-4-ขา-ผสมสี)
   * [2.3 จอ 7-Segment 1 หลัก (10 ขา)](#23-จอ-7-segment-1-หลัก-10-ขา)
   * [2.4 จอ 7-Segment 2 หลัก (Multiplexing)](#24-จอ-7-segment-2-หลัก-multiplexing)
   * [2.5 จอ I2C LCD 16x2](#25-จอ-i2c-lcd-16x2)
3. [หมวดปุ่มกด & เซ็นเซอร์ทั้งหมด (Inputs — มักออกฝั่ง Publish)](#3-หมวดปุ่มกด--เซ็นเซอร์ทั้งหมด-inputs)
   * [3.1 ปุ่มกด (Push Button — ทั้งแบบไม่ใช้ R และใช้ R)](#31-ปุ่มกด-push-button)
   * [3.2 ตัวต้านทานปรับค่าได้ (Potentiometer 3 ขา) & วัดแสง (LDR 2 ขา)](#32-ตัวต้านทานปรับค่าได้-potentiometer--วัดแสง-ldr)
   * [3.3 เซ็นเซอร์อุณหภูมิแบบ 3 ขา (TMP36 / MCP9700 / LM35)](#33-เซ็นเซอร์อุณหภูมิแบบ-3-ขา-tmp36--mcp9700)
   * [3.4 เซ็นเซอร์อุณหภูมิแบบ 2 ขา (Thermistor NTC)](#34-เซ็นเซอร์อุณหภูมิแบบ-2-ขา-thermistor)
   * [3.5 เซ็นเซอร์วัดระยะทาง Ultrasonic (HC-SR04 4 ขา)](#35-เซ็นเซอร์วัดระยะทาง-ultrasonic-hc-sr04)
4. [คู่มือทำเวอร์ชัน MQTT (How-To แปลงทุกข้อเป็น MQTT Publish & Subscribe)](#4-คู่มือทำเวอร์ชัน-mqtt-how-to-mqtt-version)
5. [โค้ดแม่แบบ MQTT รวมทุกข้อในไฟล์เดียว (พร้อมสอบจริง)](#5-โค้ดแม่แบบ-mqtt-รวมทุกข้อในไฟล์เดียว)

---

## 1. ผังจัดสรรขา Master Pinout

ถ้าในห้องสอบต้องต่ออุปกรณ์ทุกชิ้นไว้บนบอร์ดเดียว ให้เสียบตามตารางนี้ **ขาจะไม่ชนกันเลย**:

| ขาบนบอร์ด UNO R4 | อุปกรณ์ที่เสียบ | โหมดขา (`pinMode`) | หมายเหตุสำคัญ |
| :--- | :--- | :--- | :--- |
| **`D2, D3, D4, D5, D6, D7, D8`** | **7-Segment (`a, b, c, d, e, f, g`)** | `OUTPUT` | ขากลาง (`COM`) ต่อ R $220\Omega$ ลง `GND` |
| **`D9, D10, D11`** | **RGB LED (`Red, Green, Blue`)** | `OUTPUT` (ขา PWM มี `~`) | ขายาวสุด (ขา 2) ต่อ `+5V` (Common Anode) |
| **`D12`** | **Ultrasonic ขา `Trig`** | `OUTPUT` | ขายิงคลื่นเสียง |
| **`D13`** | **Ultrasonic ขา `Echo`** | `INPUT` | ขารับคลื่นสะท้อน |
| **`A0`** | **Temp Sensor (TMP36 หรือ Thermistor)** | `analogRead(A0)` | อ่านค่าอุณหภูมิ |
| **`A1`** | **Potentiometer (หมุนค่า) หรือ LDR (วัดแสง)** | `analogRead(A1)` | อ่านค่า `0 - 1023` |
| **`A2`** | **Push Button (ปุ่มกด)** | `INPUT_PULLUP` | อีกขาของปุ่มเสียบลง `GND` เลย (กด = `LOW`) |
| **`A4 (SDA), A5 (SCL)`** | **จอ I2C LCD 16x2** | I2C (`Wire`) | `VCC` ต่อ `5V`, `GND` ต่อ `GND` |

> [!CAUTION]
> **ห้ามเสียบสายที่ขา `D0 (RX)` และ `D1 (TX)`** เด็ดขาดถ้าใช้ `Serial.begin(9600)` หรืออัปโหลดโค้ด เพราะมันเป็นพอร์ตสื่อสาร USB!

---

## 2. หมวดไฟ & จอแสดงผล (Outputs)

---

### 2.1 หลอดไฟ LED เดี่ยว (เปิด/ปิด & หรี่ไฟ PWM)

#### วิธีดูขาและต่อวงจร
* **ขายาว (`+` Anode):** ต่อเข้าขา Arduino (ถ้าจะหรี่ไฟต้องเสียบขาที่มีเครื่องหมาย `~` ได้แก่ **`3, 5, 6, 9, 10, 11`**)
* **ขาสั้น (`-` Cathode / ด้านแบนของหัวหลอด):** ต่อผ่านตัวต้านทาน **$220\Omega$ – $330\Omega$** ลง **`GND`**

#### โค้ดแยกเดี่ยว (เปิด-ปิด และ หรี่ไฟ PWM)
```cpp
const int ledPin = 3; // ต้องเป็นขาที่มี ~ (3, 5, 6, 9, 10, 11) ถ้าจะใช้ analogWrite

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // 1. เปิด-ปิดธรรมดา (Digital)
  digitalWrite(ledPin, HIGH); // ไฟติดสุด
  delay(500);
  digitalWrite(ledPin, LOW);  // ไฟดับ
  delay(500);

  // 2. หรี่ไฟ (PWM: 0 = ดับ, 128 = สว่างครึ่งเดียว, 255 = สว่างสุด)
  analogWrite(ledPin, 128);
  delay(500);
}
```

#### โค้ดส่วน MQTT (เมื่อรับค่า Subscribe มาสั่งไฟ LED เดี่ยว)
```cpp
// ใส่ในฟังก์ชัน messageReceived(String &topic, String &payload)
if (topic == "68070141/led") {
  // กรณีที่ 1: โจทย์ส่งข้อความ "on" / "off"
  if (payload == "on" || payload == "ON" || payload == "1") {
    digitalWrite(ledPin, HIGH);
  } else if (payload == "off" || payload == "OFF" || payload == "0") {
    digitalWrite(ledPin, LOW);
  }

  // กรณีที่ 2: โจทย์ส่งตัวเลขความสว่าง 0 - 255 มาหรี่ไฟ
  // int brightness = payload.toInt();
  // analogWrite(ledPin, brightness);
}
```

---

### 2.2 หลอดไฟ RGB LED 4 ขา (ผสมสี)

#### วิธีดูขาและต่อวงจร
หันหลอดให้ **ขายาวที่สุดอยู่ตำแหน่งที่ 2 นับจากซ้าย** จะเรียงดังนี้:
* **ขาที่ 1 (ซ้ายสุด — `Red`):** ต่อตัวต้านทาน $220\Omega$ เข้าขา **`D9`**
* **ขาที่ 2 (ยาวที่สุด — `Common`):**
  * **แบบ Common Anode (ของชุดคิตที่เราใช้):** ต่อเข้า **`+5V`** (ลอจิกสลับด้าน: `0` = สว่างสุด, `255` = ดับ $\rightarrow$ ใช้สูตร `255 - ค่าสี`)
  * **แบบ Common Cathode (เผื่อเจอในห้องสอบ):** ต่อเข้า **`GND`** (ลอจิกตรง: `255` = สว่างสุด, `0` = ดับ $\rightarrow$ ส่งค่าสีตรงๆ)
* **ขาที่ 3 (`Green`):** ต่อตัวต้านทาน $220\Omega$ เข้าขา **`D10`**
* **ขาที่ 4 (ขวาสุด — `Blue`):** ต่อตัวต้านทาน $220\Omega$ เข้าขา **`D11`**

> **วิธีเช็คหน้างานใน 5 วินาทีว่าได้หลอดชนิดไหน:** เอาขา 2 เสียบ `5V` แล้วสั่ง `setColor(255, 0, 0)` ด้านล่าง ถ้าออก **สีแดง** แปลว่าเป็น **Common Anode** ถูกต้อง! (ถ้าออกสีฟ้าอมเขียว Cyan แปลว่าสลับลอจิก)

#### ตารางผสมสีที่ใช้สอบบ่อย
| สีที่โจทย์สั่ง | โค้ดที่เรียกใช้ `setColor(R, G, B)` |
| :--- | :--- |
| **สีแดง (Red)** | `setColor(255, 0, 0);` |
| **สีเขียว (Green)** | `setColor(0, 255, 0);` |
| **สีน้ำเงิน/ฟ้า (Blue)** | `setColor(0, 0, 255);` |
| **สีเหลือง (Yellow = แดง+เขียว)** | `setColor(255, 255, 0);` |
| **สีม่วง (Magenta/Purple = แดง+น้ำเงิน)** | `setColor(255, 0, 255);` |
| **สีฟ้าอ่อน (Cyan = เขียว+น้ำเงิน)** | `setColor(0, 255, 255);` |
| **สีขาว (White = เปิดทุกสี)** | `setColor(255, 255, 255);` |
| **ปิดไฟ (Off)** | `setColor(0, 0, 0);` |

#### โค้ดแยกเดี่ยว (RGB LED)
```cpp
const int redPin   = 9;
const int greenPin = 10;
const int bluePin  = 11;

void setColor(int r, int g, int b) {
  // สำหรับหลอด Common Anode (ขา 2 ต่อ 5V) ต้องใช้ 255 - ค่าสี
  // (ถ้าเจอหลอด Common Cathode ขา 2 ต่อ GND ให้ลบ "255 -" ออกเป็น analogWrite(redPin, r);)
  analogWrite(redPin,   255 - r);
  analogWrite(greenPin, 255 - g);
  analogWrite(bluePin,  255 - b);
}

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  setColor(0, 0, 0); // เริ่มต้นปิดไฟ
}

void loop() {
  setColor(255, 0, 0); // แดง
  delay(1000);
  setColor(0, 255, 0); // เขียว
  delay(1000);
  setColor(0, 0, 255); // น้ำเงิน
  delay(1000);
}
```

#### โค้ดส่วน MQTT (Subscribe ข้อ `venus` รับค่าตัวเลขแล้วเปลี่ยนสี RGB)
```cpp
// ใส่ในฟังก์ชัน messageReceived(String &topic, String &payload)
if (topic == "68070141/venus") {
  int val = payload.toInt();
  if (val >= 36 && val <= 50) {
    setColor(255, 0, 0);   // 36 - 50: สีแดง
  } else if (val >= 26 && val <= 35) {
    setColor(0, 0, 255);   // 26 - 35: สีน้ำเงิน/ฟ้า
  } else if (val >= 10 && val <= 25) {
    setColor(0, 255, 0);   // 10 - 25: สีเขียว
  } else {
    setColor(0, 0, 0);     // นอกช่วง: ปิดไฟ
  }
}
```

---

### 2.3 จอ 7-Segment 1 หลัก (10 ขา)

#### วิธีดูขาและต่อวงจร
หันจอให้ **จุดทศนิยม (`dp`) อยู่มุมขวาล่าง** จะมีขาบน 5 ขา และขาล่าง 5 ขา:
* **แถวบน (ซ้ายไปขวา):** `[1: g]` `[2: f]` `[3: COM]` `[4: a]` `[5: b]`
* **แถวล่าง (ซ้ายไปขวา):** `[6: e]` `[7: d]` `[8: COM]` `[9: c]` `[10: dp]`
* **ขา `COM` (ขากลางแถวบน หรือ ขากลางแถวล่าง เลือกต่อขาเดียวพอ):**
  * **Common Cathode (จอที่เราใช้):** ต่อตัวต้านทาน $220\Omega$ ลง **`GND`** (`1` = ติด, `0` = ดับ)
  * **Common Anode:** ต่อตัวต้านทาน $220\Omega$ เข้า **`5V`** (`0` = ติด, `1` = ดับ)
* **การเสียบสายเข้า Arduino ตาม Master Pinout:**
  * `a` $\rightarrow$ **`D2`**
  * `b` $\rightarrow$ **`D3`**
  * `c` $\rightarrow$ **`D4`**
  * `d` $\rightarrow$ **`D5`**
  * `e` $\rightarrow$ **`D6`**
  * `f` $\rightarrow$ **`D7`**
  * `g` $\rightarrow$ **`D8`**
  *(หมายเหตุ: ถ้าคุณเสียบค้างไว้ตามไฟล์ `Step2_7segment.ino` เดิมที่ใช้ขา `{7, 6, 4, 2, 1, 9, 10}` ให้ระวังว่าขา `1` จะชนกับ `Serial` และขา `9, 10` จะชนกับ `RGB LED` แนะนำให้ย้ายมาเรียง `{2, 3, 4, 5, 6, 7, 8}` จะไม่ชนกับอะไรเลย)*

#### โค้ดแยกเดี่ยว (7-Segment 1 หลัก)
```cpp
const int segPins[7] = {2, 3, 4, 5, 6, 7, 8}; // เรียง a, b, c, d, e, f, g

// ตารางตัวเลข 0-9 สำหรับ Common Cathode (1 = ติด, 0 = ดับ)
const int num_array[10][7] = {
  // a, b, c, d, e, f, g
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

void displayDigit(int digit) {
  if (digit < 0 || digit > 9) return;
  for (int i = 0; i < 7; i++) {
    // ถ้าเจอจอ Common Anode ให้เติมเครื่องหมาย ! หน้า num_array เป็น !num_array[digit][i]
    digitalWrite(segPins[i], num_array[digit][i]);
  }
}

void setup() {
  for (int i = 0; i < 7; i++) {
    pinMode(segPins[i], OUTPUT);
  }
}

void loop() {
  for (int d = 0; d <= 9; d++) {
    displayDigit(d);
    delay(1000);
  }
}
```

#### โค้ดส่วน MQTT (Subscribe ข้อ `prob_stat` รับเลข 0-9 มาโชว์)
```cpp
// ใส่ในฟังก์ชัน messageReceived(String &topic, String &payload)
if (topic == "68070141/prob_stat") {
  int digit = payload.toInt(); // แปลงข้อความที่รับมาเป็นตัวเลข
  displayDigit(digit);
}
```

---

### 2.4 จอ 7-Segment 2 หลัก (Multiplexing)

ใช้เมื่อโจทย์สั่งแสดงเลข 2 หลัก (`00` ถึง `99`) โดยสลับเปิดหลักซ้าย-ขวาเร็วๆ (จาก `Lab12/7segment_2`):

#### โค้ด 7-Segment 2 หลัก (รองรับการทำงานร่วมกับ `mqtt.loop()` โดยไม่ค้าง)
```cpp
const int segments[7] = {2, 3, 4, 5, 6, 7, 8}; // a, b, c, d, e, f, g
const int leftDigitPin  = 11; // ขาควบคุมหลักสิบ
const int rightDigitPin = 12; // ขาควบคุมหลักหน่วย

const int digitPatterns[10][7] = {
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

int currentNumber = 41; // ตัวแปรเก็บเลข 2 หลักที่ต้องการแสดง (อัปเดตค่าจาก MQTT ได้เลย)

void showOneSide(int digit, int activePin) {
  digitalWrite(leftDigitPin,  (activePin == leftDigitPin)  ? HIGH : LOW);
  digitalWrite(rightDigitPin, (activePin == rightDigitPin) ? HIGH : LOW);
  for (int i = 0; i < 7; i++) {
    digitalWrite(segments[i], digitPatterns[digit][i]);
  }
}

// เรียกฟังก์ชันนี้ใน loop() ตลอดเวลา
void refreshTwoDigits(int number) {
  int tens = (number / 10) % 10;
  int ones = number % 10;
  showOneSide(tens, rightDigitPin);
  delay(5);
  showOneSide(ones, leftDigitPin);
  delay(5);
}
```

---

### 2.5 จอ I2C LCD 16x2

#### วิธีต่อสาย (4 เส้น)
* `GND` $\rightarrow$ **`GND`**
* `VCC` $\rightarrow$ **`5V`**
* `SDA` $\rightarrow$ **`A4`** (หรือขา `SDA` มุมบนซ้ายของบอร์ด)
* `SCL` $\rightarrow$ **`A5`** (หรือขา `SCL` มุมบนซ้ายของบอร์ด)

#### โค้ดแยกเดี่ยว + MQTT Subscribe แสดงข้อความออกจอ LCD
```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // ถ้าจอไม่ติดลองเปลี่ยน 0x27 เป็น 0x3F

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0); // คอลัมน์ 0, บรรทัดบน (0)
  lcd.print("Ready...");
}

// เวลารับค่าจาก MQTT ใน messageReceived():
void showOnLCD(String line1, String line2) {
  lcd.clear();         // ล้างข้อความเก่าไม่ให้ตัวอักษรค้าง
  lcd.setCursor(0, 0); // บรรทัดที่ 1
  lcd.print(line1);
  lcd.setCursor(0, 1); // บรรทัดที่ 2
  lcd.print(line2);
}
```

---

## 3. หมวดปุ่มกด & เซ็นเซอร์ทั้งหมด (Inputs)

---

### 3.1 ปุ่มกด (Push Button)

#### วิธีต่อสายที่ง่ายและเร็วที่สุด (`INPUT_PULLUP` — ไม่ต้องใช้ตัวต้านทาน!)
1. เสียบปุ่มกดคร่อมร่องกลาง Breadboard
2. เลือก **2 ขาที่อยู่แนวทแยงมุมกัน** (ขาฝั่งตรงข้ามทแยงมุมจะตัดต่อถึงกันเมื่อกดปุ่มเสมอ ไม่ต้องกลัวหันปุ่มผิดด้าน!)
3. ขาหนึ่งต่อเข้า **`A2`** (หรือ Digital Pin ใดก็ได้)
4. อีกขาหนึ่งต่อลง **`GND`** โดยตรง

> **ลอจิกสำคัญมาก:** เมื่อใช้ `pinMode(buttonPin, INPUT_PULLUP);`
> * **ตอนกดปุ่ม:** `digitalRead(buttonPin)` จะได้ **`LOW` (`0`)**
> * **ตอนปล่อยปุ่ม:** `digitalRead(buttonPin)` จะได้ **`HIGH` (`1`)**

#### โค้ดแยกเดี่ยว (เช็คสถานะปุ่มกด)
```cpp
const int buttonPin = A2;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  bool isPressed = (digitalRead(buttonPin) == LOW);
  if (isPressed) {
    Serial.println("Button Pressed! (LOW)");
  } else {
    Serial.println("Released (HIGH)");
  }
  delay(200);
}
```

---

### 3.2 ตัวต้านทานปรับค่าได้ (Potentiometer) & วัดแสง (LDR)

#### วิธีต่อสาย
* **แบบที่ 1: Potentiometer (3 ขา หมุนค่าได้):**
  * ขาซ้าย $\rightarrow$ **`5V`**
  * ขากลาง $\rightarrow$ **`A1`** (หรือ `A0`)
  * ขาขวา $\rightarrow$ **`GND`**
* **แบบที่ 2: LDR (หัวกลม 2 ขา วัดแสง):**
  * ต่ออนุกรมกับตัวต้านทาน $10\text{k}\Omega$:
    `5V` $\rightarrow$ `ขา LDR` $\rightarrow$ **(จุดเชื่อมตรงกลางดึงสายเข้า `A1`)** $\rightarrow$ `ตัวต้านทาน 10k` $\rightarrow$ `GND`

#### โค้ดแยกเดี่ยว + แปลงค่าทุกรูปแบบที่ข้อสอบชอบถาม
```cpp
const int lightPin = A1;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int rawVal = analogRead(lightPin);          // 1. ค่าดิบ 0 - 1023 (ข้อสอบ 68070141/light ใช้ค่านี้เลย!)
  int pwmVal = rawVal / 4;                    // 2. แปลงเป็น 0 - 255 (เผื่อเอาไปสั่งความสว่าง LED)
  int percent = map(rawVal, 0, 1023, 0, 100); // 3. แปลงเป็นเปอร์เซ็นต์ 0 - 100%

  Serial.print("Raw (0-1023): "); Serial.print(rawVal);
  Serial.print(" | PWM (0-255): "); Serial.print(pwmVal);
  Serial.print(" | Percent: "); Serial.println(percent);
  delay(500);
}
```

#### โค้ดส่วน MQTT (Publish ค่า `0 - 1023` ไปที่ `68070141/light`)
```cpp
int lightVal = analogRead(lightPin);
mqtt.publish("68070141/light", String(lightVal));
```

---

### 3.3 เซ็นเซอร์อุณหภูมิแบบ 3 ขา (TMP36 / MCP9700)

#### วิธีดูขาและต่อวงจร (ระวังมาก!)
หัน **ด้านเรียบ (ด้านแบนที่มีตัวหนังสือ)** เข้าหาตัวเรา แล้วมองจากซ้ายไปขวา:
1. **ขาซ้ายสุด (`VCC`):** ต่อเข้า **`5V`**
2. **ขากลาง (`Vout`):** ต่อเข้า **`A0`**
3. **ขาขวาสุด (`GND`):** ต่อเข้า **`GND`**

> [!WARNING]
> **ห้ามสลับขาซ้าย (`5V`) กับขาขวา (`GND`) เด็ดขาด!** ถ้าเสียบกลับด้าน ตัว IC จะร้อนจัดภายในไม่กี่วินาทีและอ่านค่าเพี้ยน

#### โค้ดแยกเดี่ยว (TMP36 / MCP9700 — จาก `Lab14/Tmp36`)
```cpp
const int tempPin = A0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int raw = analogRead(tempPin);
  float voltage = raw * (5.0 / 1024.0);
  float tempC = (voltage - 0.5) * 100.0; // สูตรมาตรฐาน TMP36 / MCP9700
  // หมายเหตุ: ถ้าตัวถังเขียนว่า LM35 ให้ใช้สูตร float tempC = voltage * 100.0; (ไม่มี - 0.5)

  Serial.print("Temp: ");
  Serial.print(tempC, 1);
  Serial.println(" C");
  delay(1000);
}
```

---

### 3.4 เซ็นเซอร์อุณหภูมิแบบ 2 ขา (Thermistor)

#### วิธีต่อวงจร (จาก `Lab14/thermistor`)
ต่ออนุกรมแบ่งแรงดันกับตัวต้านทานคงที่ (`R_fixed` ในแล็บใช้ $330\Omega$ หรือ $10\text{k}\Omega$):
* `5V` $\rightarrow$ ขาที่ 1 ของ `R_fixed`
* ขาที่ 2 ของ `R_fixed` ต่อลงรูเดียวกับ **ขาที่ 1 ของ Thermistor** แล้วดึงสายจากจุดเชื่อมนี้เข้า **`A0`**
* ขาที่ 2 ของ Thermistor $\rightarrow$ **`GND`**

#### โค้ดแยกเดี่ยว (Thermistor)
```cpp
#include <math.h>
const int thermPin = A0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int adc = analogRead(thermPin);
  float Vout = adc * (5.0 / 1023.0);

  // ถ้าในห้องสอบใช้ตัวต้านทาน 10k ให้เปลี่ยน 330.0 เป็น 10000.0
  float R = (330.0 * Vout) / (5.0 - Vout);
  float tempC = (1.0 / ((1.0 / 298.15) + (log(R / 10000.0) / 3950.0))) - 273.15;

  Serial.print("Temp: ");
  Serial.print(tempC, 1);
  Serial.println(" C");
  delay(1000);
}
```

#### โค้ดส่วน MQTT (Publish อุณหภูมิไปที่ `68070141/temp`)
```cpp
float tempC = readTemperatureC();
mqtt.publish("68070141/temp", String(tempC, 1)); // ทศนิยม 1 ตำแหน่ง
```

---

### 3.5 เซ็นเซอร์วัดระยะทาง Ultrasonic (HC-SR04)

#### วิธีต่อสาย (4 ขา เรียงตามตัวอักษรหน้าบอร์ด HC-SR04)
* `VCC` $\rightarrow$ **`5V`** *(ต้องเป็น `5V` เท่านั้น ห้ามเสียบ `3.3V` ไม่งั้นได้ระยะ `0`)*
* `Trig` $\rightarrow$ **`D12`** (หรือ `D4` ถ้าใช้โค้ด `Lab14` เดิม)
* `Echo` $\rightarrow$ **`D13`** (หรือ `D2` ถ้าใช้โค้ด `Lab14` เดิม)
* `GND` $\rightarrow$ **`GND`**

> **ทำไมอ่านค่าได้ `Distance: 0`? เช็ค 3 จุดนี้หายแน่นอน:**
> 1. **เลขขาในโค้ดไม่ตรงกับที่เสียบจริง:** เช็คว่า `trigPin` กับ `echoPin` ในโค้ดตรงกับเลขช่องที่เสียบบนบอร์ดไหม และไม่ได้สลับสาย `Trig` กับ `Echo`
> 2. **ไฟไม่เข้าเซ็นเซอร์:** ขา `VCC` ต้องเสียบ `5V` แน่นๆ และ `GND` ลงกราวด์ร่วมกับบอร์ด
> 3. **หันเซ็นเซอร์ไปในที่โล่งไกลเกิน 4 เมตร:** คลื่นไม่สะท้อนกลับทำให้ `pulseIn` หมดเวลาและคืนค่า `0` ให้เอามือหรือหนังสือมาบังหน้าเซ็นเซอร์ห่างประมาณ `10 - 30 cm` ตอนทดสอบ

#### โค้ดแยกเดี่ยว: Ultrasonic + ปุ่มกด (ครบโจทย์ `food` และ `door` ใน Step 4!)
```cpp
const int trigPin   = 12; // ถ้าเสียบช่อง 4 ให้แก้เป็น 4
const int echoPin   = 13; // ถ้าเสียบช่อง 2 ให้แก้เป็น 2
const int buttonPin = A2; // ปุ่มกดต่อ A2 กับ GND

long readDistanceCm() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long pulseWidth = pulseIn(echoPin, HIGH, 30000); // รอคลื่นสะท้อนสูงสุด 30ms
  return pulseWidth / 29 / 2;                      // แปลงเป็น cm
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

  // เงื่อนไขข้อ 3 (68070141/food): > 20 cm ส่ง "off", <= 20 cm ส่ง "on" (ตัวพิมพ์เล็ก!)
  String foodMsg = (distance > 20) ? "off" : "on";

  // เงื่อนไขข้อ 4 (68070141/door): >= 20 cm และกดปุ่ม ส่ง "On", อื่นๆ ส่ง "Off" (ตัว O พิมพ์ใหญ่!)
  String doorMsg = (distance >= 20 && isPressed) ? "On" : "Off";

  Serial.print("Distance: "); Serial.print(distance);
  Serial.print(" cm | Button: "); Serial.print(isPressed ? "PRESSED" : "RELEASED");
  Serial.print(" | food: "); Serial.print(foodMsg);
  Serial.print(" | door: "); Serial.println(doorMsg);

  delay(500);
}
```

---

## 4. คู่มือทำเวอร์ชัน MQTT (How-To MQTT Version)

เวลาอยู่ในห้องสอบแล้วต้องเขียนโค้ด MQTT ส่งเข้าเว็บ `phycom.it.kmitl.ac.th` ให้ทำตาม **4 ขั้นตอนตายตัว** นี้:

### ขั้นที่ 1: เช็ค Library และตั้งค่าหัวไฟล์
1. ใน Arduino IDE เปิด **Library Manager** ค้นหาคำว่า **`MQTT`** (ผู้พัฒนา **Joel Gaehwiler**) แล้วกด Install
2. หัวไฟล์ต้องมี 2 บรรทัดนี้เสมอสำหรับบอร์ด UNO R4 WiFi:
```cpp
#include <WiFiS3.h>
#include <MQTTClient.h>
```
3. ตั้งค่า Broker และ Port:
   * **Host:** `"phycom.it.kmitl.ac.th"`
   * **Port:** **`1883`** *(ห้ามใส่ `8884` ตามหน้าเว็บเด็ดขาด! หน้าเว็บใช้ WebSocket `8884` แต่บอร์ด Arduino ใช้ TCP `1883`)*
   * **Client ID:** ตั้งให้มีรหัสนักศึกษาตัวเอง เช่น `"UnoR4_68070141"` เพื่อไม่ให้เตะกันเองกับเพื่อนในห้องสอบ

---

### ขั้นที่ 2: วิธีทำฝั่ง Subscribe (รับค่าจากเว็บมาสั่งไฟ/จอ)
ทุกครั้งที่ Server ส่งข้อมูลมา ฟังก์ชัน `messageReceived(String &topic, String &payload)` จะทำงานอัตโนมัติ:
1. **ต้องจดทะเบียน `mqtt.subscribe("ชื่อ_topic");`** ไว้ท้ายฟังก์ชัน `connect()` เสมอ (ถ้าลืมบรรทัดนี้ บอร์ดจะไม่รับค่าอะไรเลย!)
2. **วิธีแปลงค่า `payload` ที่รับเข้ามา:**
   * ถ้าเว็บส่ง **จำนวนเต็ม** (เช่น `0-9`, อุณหภูมิ `10-50`): ใช้ `int val = payload.toInt();`
   * ถ้าเว็บส่ง **ทศนิยม** (เช่น `25.5`): ใช้ `float val = payload.toFloat();`
   * ถ้าเว็บส่ง **ข้อความ** (เช่น `"on"`, `"off"`): เทียบตรงๆ ด้วย `if (payload == "on")`

---

### ขั้นที่ 3: วิธีทำฝั่ง Publish (อ่านค่าเซ็นเซอร์ส่งขึ้นเว็บ)
ใน `loop()` ห้ามใช้ `delay(2000);` ยาวๆ เพราะจะทำให้ `mqtt.loop()` หยุดทำงานและรับค่า Subscribe ไม่ทัน ให้ใช้ตัวจับเวลา `millis()` แทน:
```cpp
if (millis() - lastPublishTime >= 2000) { // ส่งทุกๆ 2 วินาที
  lastPublishTime = millis();

  // แปลงตัวเลขทุกชนิดเป็น String() แล้วส่งออกได้เลย:
  mqtt.publish("68070141/temp",  String(tempC, 1)); // ส่ง float ทศนิยม 1 ตำแหน่ง
  mqtt.publish("68070141/light", String(lightVal)); // ส่ง int
  mqtt.publish("68070141/food",  foodMsg);          // ส่ง String ("on" / "off")
  mqtt.publish("68070141/door",  doorMsg);          // ส่ง String ("On" / "Off")
}
```

---

### ขั้นที่ 4: เช็คลิสต์ก่อนกดส่งงานอาจารย์
* [ ] ชื่อ Topic มีรหัสนักศึกษา `68070141/...` สะกดตรงกับโจทย์ทุกตัวอักษร (ไม่มีช่องว่างเกิน)
* [ ] ตัวพิมพ์เล็ก-ใหญ่ของคำว่า `"on"`/`"off"` vs `"On"`/`"Off"` ตรงตามโจทย์เป๊ะ
* [ ] เครื่องหมาย `>` vs `>=` ที่เลขขอบเขต (เช่น `20 cm`) ตรงตามโจทย์เป๊ะ
* [ ] เปิด Serial Monitor (`9600 baud`) ดูว่าขึ้น `WiFi Connected!` และ `MQTT Connected!` แล้ว

---

## 5. โค้ดแม่แบบ MQTT รวมทุกข้อในไฟล์เดียว

ก๊อปปี้โค้ดด้านล่างนี้ (หรือเปิดไฟล์ `mqttsmth/03_MQTT_All_In_One_Exam/03_MQTT_All_In_One_Exam.ino`) แก้แค่ `WIFI_SSID` กับ `WIFI_PASSWORD` แล้วอัปโหลดสอบได้เลย:

```cpp
#include <WiFiS3.h>
#include <MQTTClient.h>
#include <math.h>

// ================= 1. แก้ชื่อ WiFi ตรงนี้ =================
const char WIFI_SSID[]      = "YOUR_WIFI_SSID";
const char WIFI_PASSWORD[]  = "YOUR_WIFI_PASSWORD";

const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883; // บอร์ดใช้ 1883 เสมอ!
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_Exam";

// ================= 2. ชื่อ Topics ทั้งหมด =================
const char TOPIC_TEMP[]      = "68070141/temp";
const char TOPIC_LIGHT[]     = "68070141/light";
const char TOPIC_FOOD[]      = "68070141/food";
const char TOPIC_DOOR[]      = "68070141/door";
const char TOPIC_VENUS[]     = "68070141/venus";
const char TOPIC_PROB_STAT[] = "68070141/prob_stat";

// ================= 3. กำหนดขา Pin =================
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

WiFiClient network;
MQTTClient mqtt(256);
unsigned long lastPublishTime = 0;

// ================= 4. ฟังก์ชันสั่งงานไฟ & อ่านเซ็นเซอร์ =================
void setColor(int r, int g, int b) {
  analogWrite(redPin,   255 - r); // Common Anode (ถ้า Common Cathode ให้ใช้ r, g, b ตรงๆ)
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
  if (pulseWidth == 0) return 999;
  return pulseWidth / 29 / 2;
}

float readTemperatureC() {
  // แบบที่ 1: TMP36 / MCP9700 (3 ขา)
  int raw = analogRead(tempPin);
  float voltage = raw * (5.0 / 1024.0);
  return (voltage - 0.5) * 100.0;

  /* แบบที่ 2: ถ้าได้ Thermistor (2 ขา) ให้ใช้บล็อกนี้แทน:
  int adc = analogRead(tempPin);
  float Vout = adc * (5.0 / 1023.0);
  float R = (330.0 * Vout) / (5.0 - Vout);
  return (1.0 / ((1.0 / 298.15) + (log(R / 10000.0) / 3950.0))) - 273.15;
  */
}

// ================= 5. ฟังก์ชันรับค่า Subscribe =================
void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming [" + topic + "]: " + payload);

  if (topic == TOPIC_VENUS) {
    int venusTemp = payload.toInt();
    if (venusTemp >= 36 && venusTemp <= 50) {
      setColor(255, 0, 0); // แดง
    } else if (venusTemp >= 26 && venusTemp <= 35) {
      setColor(0, 0, 255); // น้ำเงิน
    } else if (venusTemp >= 10 && venusTemp <= 25) {
      setColor(0, 255, 0); // เขียว
    }
  }

  if (topic == TOPIC_PROB_STAT) {
    int digit = payload.toInt();
    displayDigit(digit);
  }
}

// ================= 6. เชื่อมต่อ WiFi & MQTT =================
void connectToWiFiAndMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");
  }

  if (!mqtt.connected()) {
    Serial.print("Connecting MQTT...");
    while (!mqtt.connect(MQTT_CLIENT_ID)) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nMQTT Connected!");
    mqtt.subscribe(TOPIC_VENUS);
    mqtt.subscribe(TOPIC_PROB_STAT);
  }
}

// ================= 7. Setup & Loop =================
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) pinMode(segPins[i], OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  setColor(0, 0, 0);
  mqtt.begin(MQTT_BROKER, MQTT_PORT, network);
  mqtt.onMessage(messageReceived);
  connectToWiFiAndMQTT();
}

void loop() {
  mqtt.loop();
  if (!mqtt.connected()) {
    connectToWiFiAndMQTT();
  }

  if (millis() - lastPublishTime >= 2000) {
    lastPublishTime = millis();

    float tempC = readTemperatureC();
    int lightVal = analogRead(lightPin);
    long distance = readDistanceCm();
    bool isPressed = (digitalRead(buttonPin) == LOW);

    String foodMsg = (distance > 20) ? "off" : "on";
    String doorMsg = (distance >= 20 && isPressed) ? "On" : "Off";

    mqtt.publish(TOPIC_TEMP,  String(tempC, 1));
    mqtt.publish(TOPIC_LIGHT, String(lightVal));
    mqtt.publish(TOPIC_FOOD,  foodMsg);
    mqtt.publish(TOPIC_DOOR,  doorMsg);

    Serial.print("Temp: "); Serial.print(tempC, 1);
    Serial.print(" | Light: "); Serial.print(lightVal);
    Serial.print(" | Dist: "); Serial.print(distance);
    Serial.print("cm | Food: "); Serial.print(foodMsg);
    Serial.print(" | Door: "); Serial.println(doorMsg);
  }
}
```
