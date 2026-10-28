# คู่มือเจาะลึก MQTT Version (วิธีเปลี่ยนโค้ดธรรมดาเป็น MQTT ภายใน 3 นาที)

เปิดไฟล์นี้คู่กับ [doit.md](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/doit.md) เมื่อต้องการแปลงโค้ดเซ็นเซอร์หรือไฟให้ส่ง/รับค่าผ่านระบบสอบ `phycom.it.kmitl.ac.th`

---

## 1. โครงสร้างบังคับของโปรแกรม MQTT บน Arduino UNO R4 WiFi

ทุกโปรแกรมที่ใช้ MQTT บนบอร์ด UNO R4 WiFi มีโครงสร้าง **5 ส่วน** เหมือนกันทั้งหมด:

```cpp
// [ส่วนที่ 1] Include Library และตั้งค่า WiFi + Broker
#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[]      = "ชื่อ_WiFi_หรือ_Hotspot";
const char WIFI_PASSWORD[]  = "รหัสผ่าน_WiFi";
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883; // ห้ามใส่ 8884 เด็ดขาด! บอร์ดใช้ 1883 เสมอ
const char MQTT_CLIENT_ID[] = "UnoR4_68070141"; // ใส่รหัสนักศึกษาตัวเองกันซ้ำกับเพื่อน

WiFiClient network;
MQTTClient mqtt(256);
unsigned long lastPublishTime = 0;

// [ส่วนที่ 2] ฟังก์ชันรับค่า Subscribe (ทำงานอัตโนมัติเมื่อเว็บส่งข้อมูลลงมา)
void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming [" + topic + "]: " + payload);

  // ---> เอาโค้ดสั่งงานไฟ / 7-Segment / LCD มาใส่ตรงนี้ <---
}

// [ส่วนที่ 3] ฟังก์ชันเชื่อมต่อ WiFi และ MQTT + กด Subscribe
void connectToWiFiAndMQTT() {
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(1000);
  }
  while (!mqtt.connect(MQTT_CLIENT_ID)) {
    delay(500);
  }
  // ---> มีกี่ Topic ที่ต้องรับค่าจากเว็บ ให้ใส่ mqtt.subscribe(...) ตรงนี้ให้ครบ! <---
  mqtt.subscribe("68070141/venus");
  mqtt.subscribe("68070141/prob_stat");
}

// [ส่วนที่ 4] setup()
void setup() {
  Serial.begin(9600);
  // ---> ตั้งค่า pinMode(...) ของอุปกรณ์ทั้งหมดตรงนี้ <---

  mqtt.begin(MQTT_BROKER, MQTT_PORT, network);
  mqtt.onMessage(messageReceived);
  connectToWiFiAndMQTT();
}

// [ส่วนที่ 5] loop()
void loop() {
  mqtt.loop(); // ห้ามลบ!
  if (!mqtt.connected()) {
    connectToWiFiAndMQTT();
  }

  // ส่งค่าเซ็นเซอร์ขึ้นเว็บทุกๆ 2 วินาที (ห้ามใช้ delay(2000) เพราะจะบล็อกการรับค่า Subscribe)
  if (millis() - lastPublishTime >= 2000) {
    lastPublishTime = millis();
    // ---> อ่านค่าเซ็นเซอร์แล้วสั่ง mqtt.publish("ชื่อ_topic", ข้อความ) ตรงนี้ <---
  }
}
```

---

## 2. สูตรสำเร็จฝั่ง Publish (ส่งค่าจากเซ็นเซอร์ขึ้นเว็บ)

คำสั่ง `mqtt.publish(topic, payload)` รับค่าเป็น `String` ทั้งสองตัว ดังนั้นไม่ว่าอ่านค่าอะไรมา ให้ครอบด้วย `String(...)` เสมอ:

| สิ่งที่ต้องการส่ง (Publish) | โค้ดพร้อมก๊อปวางในบล็อก `if (millis() - lastPublishTime >= 2000)` |
| :--- | :--- |
| **1. ส่งเลขจำนวนเต็ม (`int` เช่น Potentiometer/LDR `0-1023`, ระยะทาง `cm`)** | `int val = analogRead(A1);`<br/>`mqtt.publish("68070141/light", String(val));` |
| **2. ส่งเลขทศนิยม (`float` เช่น อุณหภูมิ)** | `float tempC = readTemperatureC();`<br/>`mqtt.publish("68070141/temp", String(tempC, 1));` *(เลข 1 คือทศนิยม 1 ตำแหน่ง)* |
| **3. ส่งข้อความตามเงื่อนไข (`> 20` vs `<= 20` ข้อ `food`)** | `long dist = readDistanceCm();`<br/>`String msg = (dist > 20) ? "off" : "on";`<br/>`mqtt.publish("68070141/food", msg);` |
| **4. ส่งข้อความตาม 2 เงื่อนไขคู่กัน (`>= 20` และกดปุ่ม ข้อ `door`)** | `long dist = readDistanceCm();`<br/>`bool pressed = (digitalRead(A2) == LOW);`<br/>`String msg = (dist >= 20 && pressed) ? "On" : "Off";`<br/>`mqtt.publish("68070141/door", msg);` |
| **5. ส่งสถานะปุ่มกดเดี่ยวๆ** | `bool pressed = (digitalRead(A2) == LOW);`<br/>`mqtt.publish("68070141/button", pressed ? "on" : "off");` |

---

## 3. สูตรสำเร็จฝั่ง Subscribe (รับค่าจากเว็บมาสั่งไฟ/จอ)

ใส่ไว้ใน `void messageReceived(String &topic, String &payload)` **(และอย่าลืมใส่ `mqtt.subscribe("...");` ใน `connectToWiFiAndMQTT()`)**:

### รูปแบบที่ 1: รับตัวเลขเป็นช่วง (Range) แล้วสั่งสี RGB LED (ข้อ `venus`)
```cpp
if (topic == "68070141/venus") {
  int val = payload.toInt();
  if (val >= 36 && val <= 50) {
    setColor(255, 0, 0);   // 36 - 50 -> สีแดง
  } else if (val >= 26 && val <= 35) {
    setColor(0, 0, 255);   // 26 - 35 -> สีน้ำเงิน/ฟ้า
  } else if (val >= 10 && val <= 25) {
    setColor(0, 255, 0);   // 10 - 25 -> สีเขียว
  }
}
```

### รูปแบบที่ 2: รับตัวเลข `0-9` แล้วแสดงบน 7-Segment 1 หลัก (ข้อ `prob_stat`)
```cpp
if (topic == "68070141/prob_stat") {
  int digit = payload.toInt();
  displayDigit(digit);
}
```

### รูปแบบที่ 3: รับข้อความชื่อสีหรือคำสั่งเปิด-ปิดตรงๆ
```cpp
if (topic == "68070141/color") {
  if (payload == "red")        setColor(255, 0, 0);
  else if (payload == "green") setColor(0, 255, 0);
  else if (payload == "blue")  setColor(0, 0, 255);
  else if (payload == "off")   setColor(0, 0, 0);
}
```

### รูปแบบที่ 4: รับข้อความมาโชว์บนจอ I2C LCD 16x2
```cpp
if (topic == "68070141/lcd") {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Msg received:");
  lcd.setCursor(0, 1);
  lcd.print(payload);
}
```

---

## 4. วิธีแก้ปัญหาฉุกเฉินในห้องสอบ (Troubleshooting)

1. **ติด `Connecting WiFi....` ไม่ผ่านสักที:**
   * บอร์ด UNO R4 WiFi รองรับเฉพาะ **WiFi คลื่น `2.4 GHz` เท่านั้น** (ถ้าเปิด Hotspot จากมือถือ ให้เปิดโหมด *"Maximize Compatibility / ขยายความเข้ากันได้สูงสุด"* เพื่อบังคับเป็น `2.4 GHz`)
   * เช็คตัวพิมพ์เล็ก-ใหญ่ของชื่อ WiFi และรหัสผ่าน
2. **WiFi ติดแล้ว แต่ติด `Connecting MQTT....` ไม่ผ่าน:**
   * เช็คว่า `MQTT_PORT` ตั้งเป็น **`1883`** หรือยัง (ห้ามตั้ง `8884`)
   * เช็คว่าเน็ตที่ต่อออกเว็บ `phycom.it.kmitl.ac.th` ได้ไหม
3. **ต่อ MQTT ติดแล้ว ต่อไม่กี่วินาทีหลุด แล้วต่อใหม่วนไปเรื่อยๆ:**
   * `MQTT_CLIENT_ID` ซ้ำกับคนอื่นในห้องสอบ! ให้แก้ชื่อ `MQTT_CLIENT_ID` โดยเติมรหัสนักศึกษาและตัวเลขสุ่มเข้าไป เช่น `"UnoR4_68070141_99"`
4. **ฝั่ง Publish ส่งขึ้นเว็บได้ แต่ฝั่ง Subscribe ไม่ยอมทำงาน:**
   * ลืมเขียน `mqtt.subscribe("ชื่อ_topic");` ในฟังก์ชัน `connectToWiFiAndMQTT()`
   * หรือเผลอใส่ `delay(...)` นานๆ ใน `loop()` ทำให้ `mqtt.loop()` ไม่ได้ทำงาน
