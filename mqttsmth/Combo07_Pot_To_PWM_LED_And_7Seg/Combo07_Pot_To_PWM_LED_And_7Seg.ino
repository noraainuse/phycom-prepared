/*
 * [โจทย์ประยุกต์ Combo 07] Potentiometer (A1) -> หรี่ไฟ LED PWM (ขา 9) + แสดงระดับ 0-9 บน 7-Segment + ส่ง MQTT
 * - หมุน Potentiometer แล้วเห็นทั้งไฟหรี่สว่าง 0-255, ตัวเลข 0-9 บน 7-Segment และส่งค่า 0-1023 ขึ้นเว็บ
 */
#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883;
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_C07";

const int potPin     = A1;
const int pwmLedPin  = 9; // ขา PWM (~)
const int segPins[7] = {2, 3, 4, 5, 6, 7, 8};

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

void displayDigit(int digit) {
  if (digit < 0 || digit > 9) return;
  for (int i = 0; i < 7; i++) digitalWrite(segPins[i], num_array[digit][i]);
}

void connectToWiFiAndMQTT() {
  while (WiFi.status() != WL_CONNECTED) { WiFi.begin(WIFI_SSID, WIFI_PASSWORD); delay(1000); }
  while (!mqtt.connect(MQTT_CLIENT_ID)) { delay(500); }
}

void setup() {
  Serial.begin(9600);
  pinMode(pwmLedPin, OUTPUT);
  for (int i = 0; i < 7; i++) pinMode(segPins[i], OUTPUT);

  mqtt.begin(MQTT_BROKER, MQTT_PORT, network);
  connectToWiFiAndMQTT();
}

void loop() {
  mqtt.loop();
  if (!mqtt.connected()) connectToWiFiAndMQTT();

  int rawVal = analogRead(potPin);            // 0 - 1023
  int brightness = rawVal / 4;                // 0 - 255
  int level = map(rawVal, 0, 1023, 0, 9);     // 0 - 9

  analogWrite(pwmLedPin, brightness);         // สั่งหรี่ไฟทันที
  displayDigit(level);                        // แสดงระดับ 0-9 บน 7-Segment ทันที

  if (millis() - lastPublishTime >= 2000) {
    lastPublishTime = millis();
    mqtt.publish("68070141/light", String(rawVal));
    Serial.println("Raw: " + String(rawVal) + " | PWM: " + String(brightness) + " | Level: " + String(level));
  }
}
