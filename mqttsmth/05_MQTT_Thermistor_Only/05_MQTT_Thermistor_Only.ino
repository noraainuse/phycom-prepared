// โค้ดเฉพาะข้อ MQTT Thermistor (2 ขา) ส่งค่าอุณหภูมิขึ้น Topic: 68070141/temp
#include <WiFiS3.h>
#include <MQTTClient.h>
#include <math.h>

// 1. แก้ชื่อ WiFi / Hotspot มือถือ (ต้องเป็น 2.4 GHz)
const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";

// 2. ตั้งค่า MQTT Server
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883; // บอร์ด Arduino ใช้ 1883 เสมอ!
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_therm";
const char TOPIC_TEMP[]     = "68070141/temp";

const int thermPin = A1;

WiFiClient network;
MQTTClient mqtt(256);
unsigned long lastPublishTime = 0;

// ฟังก์ชันอ่านค่าอุณหภูมิจาก Thermistor (สูตรเดียวกับ Lab14 / Step 3.2)
float readThermistorC() {
  int adc = analogRead(thermPin);
  float Vout = adc * (5.0 / 1023.0);

  // ถ้าในห้องสอบใช้ตัวต้านทาน 330 โอห์ม ใช้ 330.0 (ถ้าใช้ตัวต้านทาน 10k ให้แก้ 330.0 เป็น 10000.0)
  float R = (330.0 * Vout) / (5.0 - Vout);

  // สมการ Beta คำนวณเป็นองศาเซลเซียส (C)
  float tempC = (1.0 / ((1.0 / 298.15) + (log(R / 10000.0) / 3950.0))) - 273.15;
  return tempC;
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

  // ส่งค่าอุณหภูมิทุกๆ 2 วินาที
  if (millis() - lastPublishTime >= 2000) {
    lastPublishTime = millis();

    float tempC = readThermistorC();
    String payload = String(tempC, 1); // แปลงเป็นข้อความทศนิยม 1 ตำแหน่ง เช่น "27.8"

    mqtt.publish(TOPIC_TEMP, payload);

    Serial.print("Thermistor Temp Sent [");
    Serial.print(TOPIC_TEMP);
    Serial.print("]: ");
    Serial.print(payload);
    Serial.println(" C");
  }
}
