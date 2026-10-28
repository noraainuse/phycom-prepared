/*
 * [ข้อสอบ Mock Exam ข้อ 1B] Publish อุณหภูมิ (Thermistor แบบ 2 ขา)
 * - Topic: 68070141/temp
 * - การต่อสาย: 5V -> R_fixed (330 หรือ 10k) -> [จุดเชื่อมดึงเข้า A0] -> Thermistor -> GND
 */
#include <WiFiS3.h>
#include <MQTTClient.h>
#include <math.h>

const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883; // บอร์ดใช้ 1883 เสมอ!
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_Ex1B";
const char TOPIC_TEMP[]     = "68070141/temp";

const int thermPin = A0; // ถ้าเสียบช่อง A1 ให้แก้เป็น A1

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

    int adc = analogRead(thermPin);
    float Vout = adc * (5.0 / 1023.0);
    // ถ้าใช้ R 10k ให้เปลี่ยน 330.0 เป็น 10000.0
    float R = (330.0 * Vout) / (5.0 - Vout);
    float tempC = (1.0 / ((1.0 / 298.15) + (log(R / 10000.0) / 3950.0))) - 273.15;

    String payload = String(tempC, 1);
    mqtt.publish(TOPIC_TEMP, payload);

    Serial.println("Published [68070141/temp]: " + payload);
  }
}
