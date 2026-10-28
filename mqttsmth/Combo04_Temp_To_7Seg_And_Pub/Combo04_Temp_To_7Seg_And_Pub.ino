/*
 * [โจทย์ประยุกต์ Combo 04] Temp Sensor (A0) -> แสดงเลขบน 7-Segment (2..8) + ส่ง MQTT Publish
 * - แสดงเลขหลักหน่วยของอุณหภูมิ (เช่น 28 C โชว์เลข 8) หรือแบ่งระดับ 0-9 บน 7-Segment
 */
#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883;
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_C04";

const int tempPin    = A0;
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
  for (int i = 0; i < 7; i++) pinMode(segPins[i], OUTPUT);
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

    // เอาหลักหน่วยของอุณหภูมิมาแสดงบน 7-Segment (เช่น 28.4 -> แสดงเลข 8)
    int onesDigit = ((int)tempC) % 10;
    if (onesDigit < 0) onesDigit = 0;
    displayDigit(onesDigit);

    mqtt.publish("68070141/temp", String(tempC, 1));
    Serial.println("Temp: " + String(tempC, 1) + " | 7-Seg: " + String(onesDigit));
  }
}
