# โพยสรุปเนื้อหาสอบ Physical Computing (Open-Book Cheat Sheet)

สรุปเนื้อหาครบทั้ง 4 บท (`Micro 01–04` & `MQTT`), ผังการต่อสายฮาร์ดแวร์, สูตรคำนวณ และจุดที่คนพลาดบ่อยสำหรับเปิดดูในห้องสอบ

---

## 1. ผังจัดสรรขา (Master Pinout) เมื่อต้องต่อทุกข้อพร้อมกันบนบอร์ดเดียว

| ขา Arduino UNO R4 | อุปกรณ์ | การตั้งค่า `pinMode` / หมายเหตุ |
| :--- | :--- | :--- |
| **`D2, D3, D4, D5, D6, D7, D8`** | **7-Segment (`a, b, c, d, e, f, g`)** | `OUTPUT` (ขากลาง `COM` ต่อ R $220\Omega$ ลง `GND`) |
| **`D9, D10, D11`** | **RGB LED (`Red, Green, Blue`)** | `OUTPUT` ขา PWM `~` (ขายาวขา 2 ต่อ `+5V` แบบ Common Anode) |
| **`D12`** | **Ultrasonic `Trig`** | `OUTPUT` (ขายิงคลื่นเสียง) |
| **`D13`** | **Ultrasonic `Echo`** | `INPUT` (ขารับคลื่นสะท้อน) |
| **`A0`** | **Temp Sensor (TMP36 / Thermistor)** | `analogRead(A0)` |
| **`A1`** | **Potentiometer / LDR** | `analogRead(A1)` |
| **`A2`** | **Push Button (ปุ่มกด)** | `pinMode(A2, INPUT_PULLUP)` (อีกขาของปุ่มต่อลง `GND` เลย) |
| **`A4 (SDA), A5 (SCL)`** | **จอ I2C LCD 16x2** *(เผื่อโจทย์ออกเพิ่ม)* | Address `0x27` |

> [!CAUTION]
> **ห้ามใช้ขา `D0 (RX)` และ `D1 (TX)`** ถ้ามีการเปิด `Serial.begin()` เพราะจะชนกับการสื่อสารผ่านสาย USB!

---

## 2. สรุปการต่อสาย & โค้ดของอุปกรณ์แต่ละตัว

### 2.1 RGB LED (4 ขา - ของบอร์ดนี้เป็น Common Anode)
* **การเรียงขา (หันขายาวสุดไว้ขาที่ 2 จากซ้าย):** `[1: Red] [2: Common +5V] [3: Green] [4: Blue]`
* **Common Anode (ขา 2 ต่อ `+5V`):** `0` = สว่างสุด, `255` = ดับ (ต้องใช้ `255 - ค่าสี`)
```cpp
void setColor(int r, int g, int b) {
  analogWrite(9,  255 - r);
  analogWrite(10, 255 - g);
  analogWrite(11, 255 - b);
}
```

### 2.2 7-Segment 1 หลัก (10 ขา - ของบอร์ดนี้เป็น Common Cathode)
* **การเรียงขา (หันจุด `dp` ไว้มุมขวาล่าง):**
  * **แถวบน (ซ้ายไปขวา):** `[g] [f] [COM] [a] [b]`
  * **แถวล่าง (ซ้ายไปขวา):** `[e] [d] [COM] [c] [dp]`
  * ขา `COM` (ขากลาง ช่องที่ 3) ต่อตัวต้านทาน $220\Omega$ ลง `GND`
```cpp
const int segPins[7] = {2, 3, 4, 5, 6, 7, 8}; // a, b, c, d, e, f, g
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
    digitalWrite(segPins[i], num_array[digit][i]);
  }
}
```

### 2.3 ปุ่มกด (Push Button) แบบไม่ต้องใช้ตัวต้านทาน (`INPUT_PULLUP`)
* **การต่อสาย:** เสียบ 2 ขาแนวทแยงมุม — ขาหนึ่งเข้า **`A2`**, อีกขาลง **`GND`**
* **ลอจิก:** ไม่กด = `HIGH`, **กดปุ่ม = `LOW`**
```cpp
pinMode(A2, INPUT_PULLUP);
bool isPressed = (digitalRead(A2) == LOW);
```

### 2.4 เซ็นเซอร์อุณหภูมิ (TMP36 / MCP9700 และ Thermistor)
* **แบบที่ 1: IC 3 ขา (TMP36 / MCP9700)** — หันด้านแบนเข้าหาตัว: `[ซ้าย: 5V] [กลาง: A0] [ขวา: GND]` *(ห้ามสลับซ้าย-ขวาเด็ดขาด!)*
```cpp
int raw = analogRead(A0);
float voltage = raw * (5.0 / 1024.0);
float tempC = (voltage - 0.5) * 100.0;
```
* **แบบที่ 2: Thermistor 2 ขา (NTC 10k)** — ต่ออนุกรมกับตัวต้านทานคงที่ (`R_fixed` เช่น `330.0` หรือ `10000.0`):
  `5V` $\rightarrow$ `R_fixed` $\rightarrow$ **(จุดเชื่อมเข้า `A0`)** $\rightarrow$ `Thermistor` $\rightarrow$ `GND`
```cpp
int adc = analogRead(A0);
float Vout = adc * (5.0 / 1023.0);
float R = (330.0 * Vout) / (5.0 - Vout); // เปลี่ยน 330.0 เป็น 10000.0 ถ้าใช้ R 10k
float tempC = (1.0 / ((1.0 / 298.15) + (log(R / 10000.0) / 3950.0))) - 273.15;
```

### 2.5 Ultrasonic (HC-SR04)
* **4 ขา:** `VCC (5V)`, `Trig (D12)`, `Echo (D13)`, `GND`
```cpp
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

long pulseWidth = pulseIn(echoPin, HIGH, 30000); // timeout 30ms กันค้าง
long distance = pulseWidth / 29 / 2;             // ระยะทางเป็น cm
```

### 2.6 จอ I2C LCD 16x2 (เผื่อโจทย์ออกเพิ่ม)
* **4 ขา:** `GND`, `VCC (5V)`, `SDA (ต่อ A4)`, `SCL (ต่อ A5)`
```cpp
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ใน setup():
lcd.init();
lcd.backlight();
lcd.setCursor(0, 0); // คอลัมน์ 0, แถวบน (0)
lcd.print("Hello");
```

---

## 3. จุดตายในห้องสอบ MQTT ที่ต้องเช็คทุกครั้งก่อนส่ง

1. **Port ของ MQTT:** บนหน้าเว็บสอบแสดง `8884` (WebSocket) แต่ในโค้ด Arduino **ต้องใส่ `1883`** เสมอ!
2. **ตัวพิมพ์เล็ก-ใหญ่ของข้อความที่ Publish:**
   * เช็คโจทย์ทุกข้อว่าต้องการ `"on"` / `"off"` (พิมพ์เล็ก) หรือ `"On"` / `"Off"` (O พิมพ์ใหญ่)
3. **เครื่องหมาย `>` vs `>=`:**
   * เช็คที่เลขขอบเขต (เช่น `20 cm`) ว่าโจทย์ใช้คำว่า *"มากกว่า 20"* (`> 20`) หรือ *"มากกว่าหรือเท่ากับ 20"* (`>= 20`)
4. **การแปลงค่า Payload:**
   * **ตอน Publish (ส่งตัวเลขออกไป):** แปลงตัวเลขเป็น `String(val)` แล้วส่ง `mqtt.publish(topic, String(val))` ได้เลย!
   * **ตอน Subscribe (รับข้อความเข้ามา):** ใน `messageReceived(String &topic, String &payload)` ใช้ `int val = payload.toInt();` (หรือ `payload.toFloat();`)
5. **Library ที่ต้องติดตั้งใน Arduino IDE บนเครื่องสอบ:**
   * ค้นหาใน Library Manager ชื่อ **`MQTT`** (โดย **Joel Gaehwiler** / `256dpi`) แล้วกด Install
