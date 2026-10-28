ๅ// โค้ดเฉพาะข้อ MQTT Temp Sensor (ส่งค่าอุณหภูมิขึ้น Topic: 68070141/temp)
#include <WiFiS3.h>
#include <MQTTClient.h>
#include <math.h>

// 1. แก้ชื่อ WiFi / Hotspot มือถือ (ต้องเป็น 2.4 GHz)
const char WIFI_SSID[]      = "YOUR_WIFI_SSID";
const char WIFI_PASSWORD[]  = "YOUR_WIFI_PASSWORD";

// 2. ตั้งค่า MQTT Server ของคณะ
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883; // บอร์ด Arduino ใช้ 1883 เสมอ!
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_temp";
const char TOPIC_TEMP[]     = "68070141/temp";

const int tempPin = A0;

WiFiClient network;
MQTTClient mqtt(256);
unsigned long lastPublishTime = 0;

// ฟังก์ชันอ่านค่าอุณหภูมิ (เลือกเปิดใช้ตามชนิดเซ็นเซอร์ที่ได้ในห้องสอบ)
float readTemperatureC() {
  // --- แบบที่ 1: TMP36 / MCP9700 (หัวดำ 3 ขา เหมือนทรานซิสเตอร์) ---
  int raw = analogRead(tempPin);
  float voltage = raw * (5.0 / 1024.0);
  float tempC = (voltage - 0.5) * 100.0;
  return tempC;

  /* --- แบบที่ 2: Thermistor (2 ขา ต่ออนุกรมกับ R 330 โอห์ม) ---
  int adc = analogRead(tempPin);
  float Vout = adc * (5.0 / 1023.0);
  float R = (330.0 * Vout) / (5.0 - Vout); // ถ้าใช้ R 10k เปลี่ยน 330.0 เป็น 10000.0
  float tempC = (1.0 / ((1.0 / 298.15) + (log(R / 10000.0) / 3950.0))) - 273.15;
  return tempC;
  */
}

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
  }
}

void setup() {
  Serial.begin(9600);
  mqtt.begin(MQTT_BROKER, MQTT_PORT, network);
  connectToWiFiAndMQTT();
}

void loop() {
  mqtt.loop();
  if (!mqtt.connected()) {
    connectToWiFiAndMQTT();
  }

  // ส่งค่าอุณหภูมิขึ้น Server ทุกๆ 2 วินาที
  if (millis() - lastPublishTime >= 2000) {
    lastPublishTime = millis();

    float tempC = readTemperatureC();
    String payload = String(tempC, 1); // แปลง float เป็นข้อความ (ทศนิยม 1 ตำแหน่ง เช่น "28.5")

    mqtt.publish(TOPIC_TEMP, payload); // ส่งขึ้น Topic "68070141/temp"

    Serial.print("Published to [");
    Serial.print(TOPIC_TEMP);
    Serial.print("]: ");
    Serial.println(payload);
  }
}
