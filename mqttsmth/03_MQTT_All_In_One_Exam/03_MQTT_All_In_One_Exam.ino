// โค้ดสอบจริง MQTT รวมครบทั้ง 6 ข้อ (Publish 4 ข้อ + Subscribe 2 ข้อ)
#include <WiFiS3.h>
#include <MQTTClient.h>
#include <math.h>

// ================= 1. แก้ชื่อ WiFi ตรงนี้ =================
const char WIFI_SSID[]      = "YOUR_WIFI_SSID";
const char WIFI_PASSWORD[]  = "YOUR_WIFI_PASSWORD";

const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883; // บอร์ด Arduino ต้องใช้ 1883 เสมอ!
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_Exam";

// ================= 2. ชื่อ Topics ทั้งหมด =================
const char TOPIC_TEMP[]      = "68070141/temp";
const char TOPIC_LIGHT[]     = "68070141/light";
const char TOPIC_FOOD[]      = "68070141/food";
const char TOPIC_DOOR[]      = "68070141/door";
const char TOPIC_VENUS[]     = "68070141/venus";
const char TOPIC_PROB_STAT[] = "68070141/prob_stat";

// ================= 3. กำหนดขา Pin =================
const int segPins[7] = {2, 3, 4, 5, 6, 7, 8}; // a, b, c, d, e, f, g
const int redPin     = 9;
const int greenPin   = 10;
const int bluePin    = 11;
const int trigPin    = 12;
const int echoPin    = 13;
const int tempPin    = A0;
const int lightPin   = A1;
const int buttonPin  = A2;

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

// ================= 4. ฟังก์ชันสั่งงานไฟ & อ่านเซ็นเซอร์ =================
void setColor(int r, int g, int b) {
  analogWrite(redPin,   255 - r); // Common Anode (ถ้าเจอ Common Cathode ให้ลบ 255 - ออก)
  analogWrite(greenPin, 255 - g);
  analogWrite(bluePin,  255 - b);
}

void displayDigit(int digit) {
  if (digit < 0 || digit > 9) return;
  for (int i = 0; i < 7; i++) {
    digitalWrite(segPins[i], num_array[digit][i]);
  }
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

float readTemperatureC() {
  // แบบที่ 1: TMP36 / MCP9700 (3 ขา)
  int raw = analogRead(tempPin);
  float voltage = raw * (5.0 / 1024.0);
  return (voltage - 0.5) * 100.0;

  /* แบบที่ 2: ถ้าได้ Thermistor (2 ขา) ให้ใช้บล็อกนี้แทน:
  int adc = analogRead(tempPin);
  float Vout = adc * (5.0 / 1023.0);
  float R = (330.0 * Vout) / (5.0 - Vout);
  return (1.0 / ((1.0 / 298.15) + (log(R / 10000.0) / 3950.0))) - 273.15;
  */
}

// ================= 5. ฟังก์ชันรับค่า Subscribe =================
void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming [" + topic + "]: " + payload);

  if (topic == TOPIC_VENUS) {
    int venusTemp = payload.toInt();
    if (venusTemp >= 36 && venusTemp <= 50) {
      setColor(255, 0, 0); // 36 - 50 -> แดง
    } else if (venusTemp >= 26 && venusTemp <= 35) {
      setColor(0, 0, 255); // 26 - 35 -> น้ำเงิน
    } else if (venusTemp >= 10 && venusTemp <= 25) {
      setColor(0, 255, 0); // 10 - 25 -> เขียว
    }
  }

  if (topic == TOPIC_PROB_STAT) {
    int digit = payload.toInt();
    displayDigit(digit);
  }
}

// ================= 6. เชื่อมต่อ WiFi & MQTT =================
void connectToWiFiAndMQTT() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");
  }

  if (!mqtt.connected()) {
    Serial.print("Connecting MQTT...");
    while (!mqtt.connect(MQTT_CLIENT_ID)) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nMQTT Connected!");
    mqtt.subscribe(TOPIC_VENUS);
    mqtt.subscribe(TOPIC_PROB_STAT);
  }
}

// ================= 7. Setup & Loop =================
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) pinMode(segPins[i], OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  setColor(0, 0, 0);
  mqtt.begin(MQTT_BROKER, MQTT_PORT, network);
  mqtt.onMessage(messageReceived);
  connectToWiFiAndMQTT();
}

void loop() {
  mqtt.loop();
  if (!mqtt.connected()) {
    connectToWiFiAndMQTT();
  }

  if (millis() - lastPublishTime >= 2000) {
    lastPublishTime = millis();

    float tempC = readTemperatureC();
    int lightVal = analogRead(lightPin);
    long distance = readDistanceCm();
    bool isPressed = (digitalRead(buttonPin) == LOW);

    String foodMsg = (distance > 20) ? "off" : "on";
    String doorMsg = (distance >= 20 && isPressed) ? "On" : "Off";

    mqtt.publish(TOPIC_TEMP,  String(tempC, 1));
    mqtt.publish(TOPIC_LIGHT, String(lightVal));
    mqtt.publish(TOPIC_FOOD,  foodMsg);
    mqtt.publish(TOPIC_DOOR,  doorMsg);

    Serial.print("Temp: "); Serial.print(tempC, 1);
    Serial.print(" | Light: "); Serial.print(lightVal);
    Serial.print(" | Dist: "); Serial.print(distance);
    Serial.print("cm | Food: "); Serial.print(foodMsg);
    Serial.print(" | Door: "); Serial.println(doorMsg);
  }
}
