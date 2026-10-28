/*
 * [ข้อสอบ Mock Exam ข้อ 2] Publish ตัวต้านทานปรับค่าได้ (Potentiometer / LDR) ค่า 0 - 1023
 * - Topic: 68070141/light
 * - การต่อสาย: [ขาซ้าย: 5V] [ขากลาง: A1 (หรือ A0)] [ขาขวา: GND]
 */
#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883; // บอร์ดใช้ 1883 เสมอ!
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_Ex2";
const char TOPIC_LIGHT[]    = "68070141/light";

const int lightPin = A1; // ถ้าเสียบที่ A0 ให้แก้เป็น A0

WiFiClient network;
MQTTClient mqtt(256);
unsigned long lastPublishTime = 0;

void connectToWiFiAndMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    Serial.println("\nWiFi Connected!");
  }
  if (!mqtt.connected()) {
    Serial.print("Connecting MQTT...");
    while (!mqtt.connect(MQTT_CLIENT_ID)) { delay(500); Serial.print("."); }
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
  if (!mqtt.connected()) connectToWiFiAndMQTT();

  if (millis() - lastPublishTime >= 2000) {
    lastPublishTime = millis();

    int rawVal = analogRead(lightPin); // ค่า 0 - 1023
    String payload = String(rawVal);
    mqtt.publish(TOPIC_LIGHT, payload);

    Serial.println("Published [68070141/light]: " + payload);
  }
}
