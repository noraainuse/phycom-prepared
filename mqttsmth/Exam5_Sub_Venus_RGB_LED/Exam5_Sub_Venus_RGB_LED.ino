/*
 * [ข้อสอบ Mock Exam ข้อ 5] Subscribe อุณหภูมิดาวศุกร์ แสดงสีออก RGB LED
 * - Topic: 68070141/venus
 * - เงื่อนไข:
 *   36 - 50 -> สีแดง (255, 0, 0)
 *   26 - 35 -> สีฟ้า/น้ำเงิน (0, 0, 255)
 *   10 - 25 -> สีเขียว (0, 255, 0)
 * - การต่อสาย (Common Anode ขายาวขา 2 ต่อ +5V): Red->9, Green->10, Blue->11
 */
#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883; // บอร์ดใช้ 1883 เสมอ!
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_Ex5";
const char TOPIC_VENUS[]    = "68070141/venus";

const int redPin   = 9;
const int greenPin = 10;
const int bluePin  = 11;

WiFiClient network;
MQTTClient mqtt(256);

void setColor(int r, int g, int b) {
  // สำหรับหลอด Common Anode (ถ้าเจอ Common Cathode ต่อ GND ให้ลบ "255 -" ออก)
  analogWrite(redPin,   255 - r);
  analogWrite(greenPin, 255 - g);
  analogWrite(bluePin,  255 - b);
}

void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming [" + topic + "]: " + payload);

  if (topic == TOPIC_VENUS) {
    int temp = payload.toInt();
    if (temp >= 36 && temp <= 50) {
      setColor(255, 0, 0); // แดง
    } else if (temp >= 26 && temp <= 35) {
      setColor(0, 0, 255); // ฟ้า/น้ำเงิน
    } else if (temp >= 10 && temp <= 25) {
      setColor(0, 255, 0); // เขียว
    }
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
    mqtt.subscribe(TOPIC_VENUS); // กดติดตาม Topic venus
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  setColor(0, 0, 0);

  mqtt.begin(MQTT_BROKER, MQTT_PORT, network);
  mqtt.onMessage(messageReceived);
  connectToWiFiAndMQTT();
}

void loop() {
  mqtt.loop();
  if (!mqtt.connected()) connectToWiFiAndMQTT();
}
