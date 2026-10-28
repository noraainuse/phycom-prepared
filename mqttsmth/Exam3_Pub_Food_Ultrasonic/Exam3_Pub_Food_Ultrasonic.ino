/*
 * [ข้อสอบ Mock Exam ข้อ 3] Publish ระยะทาง Ultrasonic (HC-SR04)
 * - Topic: 68070141/food
 * - เงื่อนไข: > 20 cm ส่ง "off", <= 20 cm ส่ง "on" (ตัวพิมพ์เล็กทั้งหมด!)
 * - การต่อสาย: VCC->5V, Trig->12 (หรือ 4), Echo->13 (หรือ 2), GND->GND
 */
#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883; // บอร์ดใช้ 1883 เสมอ!
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_Ex3";
const char TOPIC_FOOD[]     = "68070141/food";

const int trigPin = 12; // ถ้าเสียบช่อง 4 ให้แก้เป็น 4
const int echoPin = 13; // ถ้าเสียบช่อง 2 ให้แก้เป็น 2

WiFiClient network;
MQTTClient mqtt(256);
unsigned long lastPublishTime = 0;

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
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  mqtt.begin(MQTT_BROKER, MQTT_PORT, network);
  connectToWiFiAndMQTT();
}

void loop() {
  mqtt.loop();
  if (!mqtt.connected()) connectToWiFiAndMQTT();

  if (millis() - lastPublishTime >= 2000) {
    lastPublishTime = millis();

    long dist = readDistanceCm();
    String payload = (dist > 20) ? "off" : "on";
    mqtt.publish(TOPIC_FOOD, payload);

    Serial.print("Distance: "); Serial.print(dist);
    Serial.println(" cm -> Published [68070141/food]: " + payload);
  }
}
