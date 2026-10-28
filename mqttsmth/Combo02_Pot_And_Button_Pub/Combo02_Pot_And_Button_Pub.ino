/*
 * [โจทย์ประยุกต์ Combo 02] Potentiometer/LDR (A1) + ปุ่มกด (A2) -> MQTT Publish
 * รองรับ 2 เงื่อนไขยอดฮิต:
 *   แบบ A: ถ้าค่าแสง/ตัวต้านทาน >= 500 และกดปุ่ม ส่ง "On", กรณีอื่นส่ง "Off"
 *   แบบ B: ถ้ากดปุ่มส่งค่า 0-1023, ถ้าไม่กดปุ่มส่ง "0" (หรือกดปุ่มเพื่อล็อกค่า)
 */
#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883;
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_C02";

const int lightPin  = A1;
const int buttonPin = A2;

WiFiClient network;
MQTTClient mqtt(256);
unsigned long lastPublishTime = 0;

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

  if (millis() - lastPublishTime >= 2000) {
    lastPublishTime = millis();

    int rawVal = analogRead(lightPin); // 0 - 1023
    bool isPressed = (digitalRead(buttonPin) == LOW);

    // แบบ A: ค่า >= 500 และกดปุ่ม ส่ง "On", อื่นๆ ส่ง "Off"
    String statusMsg = (rawVal >= 500 && isPressed) ? "On" : "Off";
    mqtt.publish("68070141/door", statusMsg);

    // ส่งค่าตัวต้านทาน 0-1023 ขึ้น 68070141/light
    mqtt.publish("68070141/light", String(rawVal));

    Serial.print("Light: "); Serial.print(rawVal);
    Serial.print(" | Button: "); Serial.print(isPressed ? "PRESSED" : "RELEASED");
    Serial.println(" | Status: " + statusMsg);
  }
}
