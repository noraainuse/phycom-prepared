/*
 * [โจทย์ประยุกต์ Combo 10] Sensor (Temp/Ultrasonic) + จอ I2C LCD 16x2 + MQTT (ตามสไลด์หน้า 68)
 * - บรรทัดบนของจอ LCD แสดงค่าอุณหภูมิและระยะทางที่อ่านได้จากบอร์ด (พร้อม Publish ขึ้นเว็บ)
 * - บรรทัดล่างของจอ LCD แสดงค่าที่ได้รับมาจาก MQTT Subscribe (เช่น อุณหภูมิดาวศุกร์ venus)
 * - การต่อสายจอ I2C LCD: GND->GND, VCC->5V, SDA->A4, SCL->A5
 */
#include <WiFiS3.h>
#include <MQTTClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883;
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_C10";

LiquidCrystal_I2C lcd(0x27, 16, 2); // ถ้าจอไม่ติดเปลี่ยน 0x27 เป็น 0x3F
const int tempPin = A0;

WiFiClient network;
MQTTClient mqtt(256);
unsigned long lastPublishTime = 0;
String lastSubMsg = "Waiting...";

void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming [" + topic + "]: " + payload);
  lastSubMsg = payload;

  lcd.setCursor(0, 1);
  lcd.print("Sub:            "); // ล้างตัวอักษรเก่า
  lcd.setCursor(5, 1);
  lcd.print(lastSubMsg);
}

void connectToWiFiAndMQTT() {
  while (WiFi.status() != WL_CONNECTED) { WiFi.begin(WIFI_SSID, WIFI_PASSWORD); delay(1000); }
  while (!mqtt.connect(MQTT_CLIENT_ID)) { delay(500); }
  mqtt.subscribe("68070141/venus");
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");

  mqtt.begin(MQTT_BROKER, MQTT_PORT, network);
  mqtt.onMessage(messageReceived);
  connectToWiFiAndMQTT();

  lcd.clear();
}

void loop() {
  mqtt.loop();
  if (!mqtt.connected()) connectToWiFiAndMQTT();

  if (millis() - lastPublishTime >= 2000) {
    lastPublishTime = millis();

    int raw = analogRead(tempPin);
    float voltage = raw * (5.0 / 1024.0);
    float tempC = (voltage - 0.5) * 100.0;

    mqtt.publish("68070141/temp", String(tempC, 1));

    // แสดงค่าอุณหภูมิบรรทัดบนของจอ LCD
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(tempC, 1);
    lcd.print(" C   ");
  }
}
