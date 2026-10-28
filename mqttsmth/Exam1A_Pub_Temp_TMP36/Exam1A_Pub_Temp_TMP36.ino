/*
 * [ข้อสอบ Mock Exam ข้อ 1A] Publish อุณหภูมิ (TMP36 / MCP9700 แบบ 3 ขา)
 * - Topic: 68070141/temp
 * - การต่อสาย (หันด้านแบนเข้าหาตัว): [ซ้าย: 5V] [กลาง: A0] [ขวา: GND]
 */
#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883; // บอร์ดใช้ 1883 เสมอ!
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_Ex1A";
const char TOPIC_TEMP[]     = "68070141/temp";

const int tempPin = A0;

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

    int raw = analogRead(tempPin);
    float voltage = raw * (5.0 / 1024.0);
    float tempC = (voltage - 0.5) * 100.0;

    String payload = String(tempC, 1);
    mqtt.publish(TOPIC_TEMP, payload);

    Serial.println("Published [68070141/temp]: " + payload);
  }
}
