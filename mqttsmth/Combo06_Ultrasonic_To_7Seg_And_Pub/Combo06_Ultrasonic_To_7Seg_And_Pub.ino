/*
 * [โจทย์ประยุกต์ Combo 06] Ultrasonic (12,13) -> แสดงระดับระยะทาง 0-9 บน 7-Segment (2..8) + ส่ง MQTT
 * - แปลงระยะทาง cm (เช่น หาร 5 หรือ constrain 0-9) มาโชว์บน 7-Segment พร้อมส่งค่าขึ้น MQTT
 */
#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883;
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_C06";

const int trigPin    = 12;
const int echoPin    = 13;
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
  for (int i = 0; i < 7; i++) pinMode(segPins[i], OUTPUT);

  mqtt.begin(MQTT_BROKER, MQTT_PORT, network);
  connectToWiFiAndMQTT();
}

void loop() {
  mqtt.loop();
  if (!mqtt.connected()) connectToWiFiAndMQTT();

  if (millis() - lastPublishTime >= 2000) {
    lastPublishTime = millis();

    long dist = readDistanceCm();
    // แปลงระยะ 0 - 45 cm เป็นตัวเลขระดับ 0 - 9 บน 7-Segment
    int digit = constrain(dist / 5, 0, 9);
    displayDigit(digit);

    String foodMsg = (dist > 20) ? "off" : "on";
    mqtt.publish("68070141/food", foodMsg);
    Serial.println("Dist: " + String(dist) + " cm -> 7-Seg: " + String(digit));
  }
}
