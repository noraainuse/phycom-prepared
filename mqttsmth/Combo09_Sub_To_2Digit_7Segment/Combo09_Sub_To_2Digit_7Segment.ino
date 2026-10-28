/*
 * [โจทย์ประยุกต์ Combo 09] MQTT Subscribe -> แสดงตัวเลข 2 หลัก (00 - 99) บน 7-Segment 2 หลัก
 * - ประยุกต์จาก Lab12/7segment_2 โดยทำงานร่วมกับ mqtt.loop() ได้ลื่นไหลไม่กระพริบ!
 * - รับค่าตัวเลข 0 - 99 จาก Topic 68070141/venus หรือ 68070141/prob_stat มาโชว์เป็นเลข 2 หลัก
 */
#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883;
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_C09";

const int segments[7]  = {2, 3, 4, 5, 6, 7, 8}; // a, b, c, d, e, f, g
const int leftSegment  = 11;
const int rightSegment = 12;

const int digitPatterns[10][7] = {
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

int currentNumber = 41; // ค่าเริ่มต้นที่จะแสดงบนจอ 2 หลัก

WiFiClient network;
MQTTClient mqtt(256);

void displaySide(int digit, int activeSegment) {
  digitalWrite(leftSegment,  activeSegment == leftSegment  ? HIGH : LOW);
  digitalWrite(rightSegment, activeSegment == rightSegment ? HIGH : LOW);
  for (int i = 0; i < 7; i++) {
    digitalWrite(segments[i], digitPatterns[digit][i] ? HIGH : LOW);
  }
}

void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming [" + topic + "]: " + payload);
  currentNumber = constrain(payload.toInt(), 0, 99);
}

void connectToWiFiAndMQTT() {
  while (WiFi.status() != WL_CONNECTED) { WiFi.begin(WIFI_SSID, WIFI_PASSWORD); delay(1000); }
  while (!mqtt.connect(MQTT_CLIENT_ID)) { delay(500); }
  mqtt.subscribe("68070141/venus");
  mqtt.subscribe("68070141/prob_stat");
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) pinMode(segments[i], OUTPUT);
  pinMode(leftSegment, OUTPUT);
  pinMode(rightSegment, OUTPUT);

  mqtt.begin(MQTT_BROKER, MQTT_PORT, network);
  mqtt.onMessage(messageReceived);
  connectToWiFiAndMQTT();
}

void loop() {
  mqtt.loop();
  if (!mqtt.connected()) connectToWiFiAndMQTT();

  // สลับแสดงหลักสิบและหลักหน่วยด้วยความเร็วสูง (Multiplexing)
  int tens = (currentNumber / 10) % 10;
  int ones = currentNumber % 10;
  displaySide(tens, rightSegment);
  delay(5);
  displaySide(ones, leftSegment);
  delay(5);
}
