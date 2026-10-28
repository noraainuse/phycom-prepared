/*
 * [โจทย์ประยุกต์ Combo 05] Ultrasonic (12,13) -> แสดงสีเตือนระยะใกล้/ไกลบน RGB LED (9,10,11) + ส่ง MQTT
 * - ระยะ <= 10 cm : สีแดง (ใกล้มาก!)
 * - ระยะ 11 - 20 cm : สีเขียว (ระยะพอดี -> ส่ง "on")
 * - ระยะ > 20 cm : สีน้ำเงิน (ไกล -> ส่ง "off")
 */
#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883;
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_C05";

const int trigPin  = 12;
const int echoPin  = 13;
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
  while (WiFi.status() != WL_CONNECTED) { WiFi.begin(WIFI_SSID, WIFI_PASSWORD); delay(1000); }
  while (!mqtt.connect(MQTT_CLIENT_ID)) { delay(500); }
}

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
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

    long dist = readDistanceCm();

    if (dist <= 10) {
      setColor(255, 0, 0); // แดง
    } else if (dist <= 20) {
      setColor(0, 255, 0); // เขียว
    } else {
      setColor(0, 0, 255); // น้ำเงิน
    }

    String foodMsg = (dist > 20) ? "off" : "on";
    mqtt.publish("68070141/food", foodMsg);
    Serial.println("Dist: " + String(dist) + " cm | Food: " + foodMsg);
  }
}
