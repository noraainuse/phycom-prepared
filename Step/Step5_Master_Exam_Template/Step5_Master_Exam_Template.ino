/*
 * ============================================================================
 * MASTER EXAM TEMPLATE: KMITL Physical Computing MQTT Exam (UNO R4 WiFi)
 * รหัสนักศึกษา: 68070141
 * ============================================================================
 * [ผังการต่อสายบนบอร์ดเดียวครบทุกข้อ]
 * - 7-Segment (Common Cathode): D2..D8 -> ขีด a..g (ขา COM ต่อ R 220 ลง GND)
 * - RGB LED (Common Anode):     D9=Red, D10=Green, D11=Blue (ขายาวขา 2 ต่อ +5V)
 * - Ultrasonic (HC-SR04):       D12=Trig, D13=Echo, VCC=5V, GND=GND
 * - Temp Sensor (TMP36/Therm):  A0
 * - Potentiometer / LDR:        A1
 * - Push Button (ปุ่มกด):       A2 กับ GND (ใช้ INPUT_PULLUP: กด = LOW)
 * ============================================================================
 */

#include <WiFiS3.h>
#include <MQTTClient.h>

// ----------------- 1. ตั้งค่า WiFi และ MQTT Broker -----------------
const char WIFI_SSID[]     = "YOUR_WIFI_SSID";     // <-- แก้ชื่อ WiFi / Hotspot มือถือ
const char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD"; // <-- แก้รหัสผ่าน WiFi

const char MQTT_BROKER[]   = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT       = 1883;                 // <-- บอร์ด Arduino ต้องใช้ 1883 เสมอ! (หน้าเว็บเป็น 8884)
const char MQTT_CLIENT_ID[]= "UnoR4_68070141_Exam"; // <-- ห้ามซ้ำกับคนอื่น

// ----------------- 2. ตั้งชื่อ Topics ตามโจทย์ -----------------
// ฝั่ง Publish (ส่งค่าจากบอร์ดขึ้น Server)
const char TOPIC_TEMP[]  = "68070141/temp";
const char TOPIC_LIGHT[] = "68070141/light";
const char TOPIC_FOOD[]  = "68070141/food";
const char TOPIC_DOOR[]  = "68070141/door";

// ฝั่ง Subscribe (รับค่าจาก Server มาแสดงผล)
const char TOPIC_VENUS[]     = "68070141/venus";
const char TOPIC_PROB_STAT[] = "68070141/prob_stat";

// ----------------- 3. กำหนดขาพิน (Master Pinout) -----------------
const int segPins[7] = {2, 3, 4, 5, 6, 7, 8}; // a, b, c, d, e, f, g
const int redPin     = 9;
const int greenPin   = 10;
const int bluePin    = 11;
const int trigPin    = 12;
const int echoPin    = 13;

const int tempPin    = A0;
const int lightPin   = A1;
const int buttonPin  = A2;

// ตาราง 7-Segment (Common Cathode: 1 = ติด, 0 = ดับ)
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

// ----------------- 4. ฟังก์ชันช่วยควบคุมอุปกรณ์ -----------------

// สั่งงาน RGB LED (สำหรับ Common Anode: 255 - ค่าสี)
void setColor(int r, int g, int b) {
  analogWrite(redPin,   255 - r);
  analogWrite(greenPin, 255 - g);
  analogWrite(bluePin,  255 - b);
}

// สั่งแสดงตัวเลข 0-9 บน 7-Segment 1 หลัก
void displayDigit(int digit) {
  if (digit < 0 || digit > 9) return;
  for (int i = 0; i < 7; i++) {
    digitalWrite(segPins[i], num_array[digit][i]);
  }
}

// อ่านระยะทางจาก Ultrasonic (หน่วย cm)
long readDistanceCm() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long pulseWidth = pulseIn(echoPin, HIGH, 30000); // timeout 30ms ป้องกันโปรแกรมค้าง
  if (pulseWidth == 0) return 999;                 // กรณีไม่มีวัตถุสะท้อนกลับ
  return pulseWidth / 29 / 2;
}

// อ่านอุณหภูมิ (°C)
float readTemperatureC() {
  // แบบที่ 1: สำหรับ TMP36 / MCP9700 (3 ขา)
  int raw = analogRead(tempPin);
  float voltage = raw * (5.0 / 1024.0);
  float tempC = (voltage - 0.5) * 100.0;
  return tempC;

  /* 
  // แบบที่ 2: ถ้าวันสอบได้ Thermistor (2 ขา) ให้คอมเมนต์แบบที่ 1 แล้วเปิดใช้บล็อกนี้แทน:
  int adc = analogRead(tempPin);
  float Vout = adc * (5.0 / 1023.0);
  float R = (330.0 * Vout) / (5.0 - Vout); // เช็คค่า R_fixed ว่าใช้ 330.0 หรือ 10000.0
  float tempC = (1.0 / ((1.0 / 298.15) + (log(R / 10000.0) / 3950.0))) - 273.15;
  return tempC;
  */
}

// ----------------- 5. ฟังก์ชันรับค่า Subscribe (Callback) -----------------
void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming [" + topic + "]: " + payload);

  // Subscribe ข้อ 1: อุณหภูมิดาวศุกร์ (68070141/venus) -> แสดงสีที่ RGB LED
  if (topic == TOPIC_VENUS) {
    int venusTemp = payload.toInt();
    if (venusTemp >= 36 && venusTemp <= 50) {
      setColor(255, 0, 0); // 36 - 50 -> สีแดง
    } else if (venusTemp >= 26 && venusTemp <= 35) {
      setColor(0, 0, 255); // 26 - 35 -> สีฟ้า/น้ำเงิน
    } else if (venusTemp >= 10 && venusTemp <= 25) {
      setColor(0, 255, 0); // 10 - 25 -> สีเขียว
    }
  }

  // Subscribe ข้อ 2: 7-Segment (68070141/prob_stat) -> แสดงเลข 0-9
  if (topic == TOPIC_PROB_STAT) {
    int digit = payload.toInt();
    displayDigit(digit);
  }
}

// ----------------- 6. ฟังก์ชันเชื่อมต่อ WiFi & MQTT -----------------
void connectToWiFiAndMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");
  }

  if (!mqtt.connected()) {
    Serial.print("Connecting to MQTT Broker...");
    while (!mqtt.connect(MQTT_CLIENT_ID)) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nMQTT Connected!");

    // กด Subscribe ทุก Topic ที่ต้องรับค่าจาก Server ทันทีที่ต่อติด
    mqtt.subscribe(TOPIC_VENUS);
    mqtt.subscribe(TOPIC_PROB_STAT);
  }
}

// ----------------- 7. Setup & Loop หลัก -----------------
void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 7; i++) {
    pinMode(segPins[i], OUTPUT);
  }
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  setColor(0, 0, 0); // ปิดไฟ RGB เริ่มต้น

  mqtt.begin(MQTT_BROKER, MQTT_PORT, network);
  mqtt.onMessage(messageReceived);

  connectToWiFiAndMQTT();
}

void loop() {
  mqtt.loop(); // สำคัญมาก! ต้องถูกเรียกตลอดเวลาเพื่อรับค่า Subscribe
  if (!mqtt.connected()) {
    connectToWiFiAndMQTT();
  }

  // ส่งค่า Publish ทั้ง 4 ข้อทุกๆ 2 วินาที (2000 ms)
  if (millis() - lastPublishTime >= 2000) {
    lastPublishTime = millis();

    // 1. Publish อุณหภูมิ (68070141/temp)
    float tempC = readTemperatureC();
    mqtt.publish(TOPIC_TEMP, String(tempC, 1));

    // 2. Publish ตัวต้านทานปรับค่าได้ 0-1023 (68070141/light)
    int lightVal = analogRead(lightPin);
    mqtt.publish(TOPIC_LIGHT, String(lightVal));

    // อ่านระยะทางและปุ่มกดสำหรับข้อ 3 และ 4
    long distance = readDistanceCm();
    bool isPressed = (digitalRead(buttonPin) == LOW);

    // 3. Publish ระยะทาง Ultrasonic (68070141/food)
    // > 20 cm ส่ง "off", <= 20 cm ส่ง "on" (ตัวพิมพ์เล็กทั้งหมด)
    String foodMsg = (distance > 20) ? "off" : "on";
    mqtt.publish(TOPIC_FOOD, foodMsg);

    // 4. Publish ประตู Ultrasonic + ปุ่ม (68070141/door)
    // >= 20 cm และกดปุ่ม ส่ง "On", กรณีอื่นส่ง "Off" (ตัว O พิมพ์ใหญ่!)
    String doorMsg = (distance >= 20 && isPressed) ? "On" : "Off";
    mqtt.publish(TOPIC_DOOR, doorMsg);

    // แสดงค่าทั้งหมดใน Serial Monitor เพื่อเช็คความถูกต้อง
    Serial.print("Temp: "); Serial.print(tempC);
    Serial.print(" | Light: "); Serial.print(lightVal);
    Serial.print(" | Dist: "); Serial.print(distance);
    Serial.print("cm | Food: "); Serial.print(foodMsg);
    Serial.print(" | Door: "); Serial.println(doorMsg);
  }
}
