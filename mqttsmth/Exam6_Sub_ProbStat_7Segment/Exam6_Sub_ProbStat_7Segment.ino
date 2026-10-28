/*
 * [ข้อสอบ Mock Exam ข้อ 6] Subscribe รับเลข 0 - 9 แสดงบน 7-Segment 1 หลัก
 * - Topic: 68070141/prob_stat
 * - การต่อสาย (Common Cathode ขากลางต่อ R 220 ลง GND):
 *   a->2, b->3, c->4, d->5, e->6, f->7, g->8
 *   (ถ้าใช้สายเดิมจาก Step 2 ให้แก้ segPins[7] เป็น {7, 6, 4, 2, 1, 9, 10})
 */
#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[]        = "68070141";
const char WIFI_PASSWORD[]    = "nano9753";
const char MQTT_BROKER[]      = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT          = 1883; // บอร์ดใช้ 1883 เสมอ!
const char MQTT_CLIENT_ID[]   = "UnoR4_68070141_Ex6";
const char TOPIC_PROB_STAT[]  = "68070141/prob_stat";

const int segPins[7] = {2, 3, 4, 5, 6, 7, 8}; // a, b, c, d, e, f, g

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

void displayDigit(int digit) {
  if (digit < 0 || digit > 9) return;
  for (int i = 0; i < 7; i++) {
    digitalWrite(segPins[i], num_array[digit][i]);
  }
}

void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming [" + topic + "]: " + payload);

  if (topic == TOPIC_PROB_STAT) {
    int digit = payload.toInt();
    displayDigit(digit);
  }
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
    mqtt.subscribe(TOPIC_PROB_STAT);
  }
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) pinMode(segPins[i], OUTPUT);

  mqtt.begin(MQTT_BROKER, MQTT_PORT, network);
  mqtt.onMessage(messageReceived);
  connectToWiFiAndMQTT();
}

void loop() {
  mqtt.loop();
  if (!mqtt.connected()) connectToWiFiAndMQTT();
}
