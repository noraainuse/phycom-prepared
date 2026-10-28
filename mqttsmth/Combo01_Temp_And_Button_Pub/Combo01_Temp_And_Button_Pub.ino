/*
 * [โจทย์ประยุกต์ Combo 01] Temp Sensor (A0) + ปุ่มกด (A2) -> MQTT Publish
 * รองรับ 3 เงื่อนไขยอดฮิตที่ข้อสอบชอบพลิกแพลง:
 *   แบบ A: ถ้าอุณหภูมิ >= 30 C และ กดปุ่ม -> ส่ง "On" (กรณีอื่นส่ง "Off")
 *   แบบ B: กดปุ่มเมื่อไหร่ ค่อยส่งค่าอุณหภูมิขึ้นเว็บทันที 1 ครั้ง
 *   แบบ C: ไม่กดปุ่มส่งหน่วย Celsius (C), ถ้ากดปุ่มค้างไว้ส่งหน่วย Fahrenheit (F)
 */
#include <WiFiS3.h>
#include <MQTTClient.h>
#include <math.h>

const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883;
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_C01";

const int tempPin   = A0;
const int buttonPin = A2; // ต่อปุ่มกดที่ A2 กับ GND

WiFiClient network;
MQTTClient mqtt(256);
unsigned long lastPublishTime = 0;
bool lastBtnState = HIGH;

float readTempC() {
  // แบบ TMP36 (3 ขา):
  int raw = analogRead(tempPin);
  float voltage = raw * (5.0 / 1024.0);
  return (voltage - 0.5) * 100.0;

  /* ถ้าใช้ Thermistor (2 ขา) ให้เปิดบล็อกนี้แทน:
  int adc = analogRead(tempPin);
  float Vout = adc * (5.0 / 1023.0);
  float R = (330.0 * Vout) / (5.0 - Vout);
  return (1.0 / ((1.0 / 298.15) + (log(R / 10000.0) / 3950.0))) - 273.15;
  */
}

void connectToWiFiAndMQTT() {
  while (WiFi.status() != WL_CONNECTED) { WiFi.begin(WIFI_SSID, WIFI_PASSWORD); delay(1000); }
  while (!mqtt.connect(MQTT_CLIENT_ID)) { delay(500); }
}

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  mqtt.begin(MQTT_BROKER, MQTT_PORT, network);
  connectToWiFiAndMQTT();
}

void loop() {
  mqtt.loop();
  if (!mqtt.connected()) connectToWiFiAndMQTT();

  bool currentBtn = digitalRead(buttonPin);
  bool isPressed  = (currentBtn == LOW);

  // แบบ B: กดปุ่ม 1 ครั้ง ส่งค่าอุณหภูมิทันที
  if (currentBtn == LOW && lastBtnState == HIGH) {
    float t = readTempC();
    mqtt.publish("68070141/temp", String(t, 1));
    Serial.println("Button Triggered Temp: " + String(t, 1));
    delay(200);
  }
  lastBtnState = currentBtn;

  // ส่งทุก 2 วินาที (แบบ A และ แบบ C)
  if (millis() - lastPublishTime >= 2000) {
    lastPublishTime = millis();
    float tempC = readTempC();
    float tempF = (tempC * 9.0 / 5.0) + 32.0;

    // แบบ A: อุณหภูมิ >= 30 และกดปุ่ม ส่ง "On", อื่นๆ ส่ง "Off"
    String alertMsg = (tempC >= 30.0 && isPressed) ? "On" : "Off";
    mqtt.publish("68070141/door", alertMsg);

    // แบบ C: ถ้ากดปุ่มส่ง Fahrenheit, ไม่กดส่ง Celsius
    float valueToSend = isPressed ? tempF : tempC;
    mqtt.publish("68070141/temp", String(valueToSend, 1));
  }
}
