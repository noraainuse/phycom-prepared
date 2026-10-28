/*
 * [โจทย์ประยุกต์ Combo 03] Temp Sensor (A0) -> แสดงสีออก RGB LED (9,10,11) + ส่ง MQTT Publish
 * (ตรงตามการทดลองในสไลด์หน้า 68: วัดอุณหภูมิแล้วแสดงสี ร้อน=แดง, กลาง=น้ำเงิน, เย็น=เขียว พร้อมส่ง MQTT)
 */
#include <WiFiS3.h>
#include <MQTTClient.h>
#include <math.h>

const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883;
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_C03";

const int tempPin  = A0;
const int redPin   = 9;
const int greenPin = 10;
const int bluePin  = 11;

WiFiClient network;
MQTTClient mqtt(256);
unsigned long lastPublishTime = 0;

void setColor(int r, int g, int b) {
  analogWrite(redPin,   255 - r); // Common Anode
  analogWrite(greenPin, 255 - g);
  analogWrite(bluePin,  255 - b);
}

float readTempC() {
  int raw = analogRead(tempPin);
  float voltage = raw * (5.0 / 1024.0);
  return (voltage - 0.5) * 100.0;
}

void connectToWiFiAndMQTT() {
  while (WiFi.status() != WL_CONNECTED) { WiFi.begin(WIFI_SSID, WIFI_PASSWORD); delay(1000); }
  while (!mqtt.connect(MQTT_CLIENT_ID)) { delay(500); }
}

void setup() {
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  setColor(0, 0, 0);

  mqtt.begin(MQTT_BROKER, MQTT_PORT, network);
  connectToWiFiAndMQTT();
}

void loop() {
  mqtt.loop();
  if (!mqtt.connected()) connectToWiFiAndMQTT();

  if (millis() - lastPublishTime >= 2000) {
    lastPublishTime = millis();

    float tempC = readTempC();

    // 1. แสดงสีที่หลอด RGB LED บนบอร์ดตามค่าอุณหภูมิที่วัดได้จริง
    if (tempC >= 32.0) {
      setColor(255, 0, 0); // ร้อน -> สีแดง
    } else if (tempC >= 27.0) {
      setColor(0, 0, 255); // ปานกลาง -> สีน้ำเงิน
    } else {
      setColor(0, 255, 0); // เย็น -> สีเขียว
    }

    // 2. ส่งค่าอุณหภูมิขึ้น MQTT
    mqtt.publish("68070141/temp", String(tempC, 1));
    Serial.println("Temp: " + String(tempC, 1) + " C");
  }
}
