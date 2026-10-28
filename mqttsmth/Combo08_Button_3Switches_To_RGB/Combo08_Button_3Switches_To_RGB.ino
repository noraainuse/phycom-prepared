/*
 * [โจทย์ประยุกต์ Combo 08] ปุ่มกด Switch + RGB LED + MQTT (ตามการทดลองในสไลด์หน้า 68 และ 75)
 * - กดปุ่มที่บอร์ดเพื่อสลับสี RGB (Red -> Green -> Blue -> Purple -> Off) พร้อม Publish ชื่อสีขึ้นเว็บ
 * - และถ้ารับข้อความ Subscribe จากเว็บ ("red", "green", "blue", "purple", "off") ก็เปลี่ยนสีไฟตามเว็บทันที!
 */
#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";
const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883;
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_C08";

const int redPin    = 9;
const int greenPin  = 10;
const int bluePin   = 11;
const int buttonPin = A2;

int colorStep = 0;
bool lastBtnState = HIGH;

WiFiClient network;
MQTTClient mqtt(256);

void setColor(int r, int g, int b) {
  analogWrite(redPin,   255 - r); // Common Anode
  analogWrite(greenPin, 255 - g);
  analogWrite(bluePin,  255 - b);
}

void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming [" + topic + "]: " + payload);

  if (payload == "red" || payload == "RED")         setColor(255, 0, 0);
  else if (payload == "green" || payload == "GREEN") setColor(0, 255, 0);
  else if (payload == "blue" || payload == "BLUE")   setColor(0, 0, 255);
  else if (payload == "purple")                      setColor(255, 0, 255);
  else if (payload == "off" || payload == "OFF")     setColor(0, 0, 0);
}

void connectToWiFiAndMQTT() {
  while (WiFi.status() != WL_CONNECTED) { WiFi.begin(WIFI_SSID, WIFI_PASSWORD); delay(1000); }
  while (!mqtt.connect(MQTT_CLIENT_ID)) { delay(500); }
  mqtt.subscribe("68070141/venus");
}

void setup() {
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  setColor(0, 0, 0);

  mqtt.begin(MQTT_BROKER, MQTT_PORT, network);
  mqtt.onMessage(messageReceived);
  connectToWiFiAndMQTT();
}

void loop() {
  mqtt.loop();
  if (!mqtt.connected()) connectToWiFiAndMQTT();

  bool currentBtn = digitalRead(buttonPin);
  if (currentBtn == LOW && lastBtnState == HIGH) {
    colorStep = (colorStep + 1) % 5;
    String colorName = "off";

    if (colorStep == 1)      { setColor(255, 0, 0);   colorName = "red"; }
    else if (colorStep == 2) { setColor(0, 255, 0);   colorName = "green"; }
    else if (colorStep == 3) { setColor(0, 0, 255);   colorName = "blue"; }
    else if (colorStep == 4) { setColor(255, 0, 255); colorName = "purple"; }
    else                     { setColor(0, 0, 0);     colorName = "off"; }

    mqtt.publish("68070141/color", colorName);
    Serial.println("Button switched color to: " + colorName);
    delay(200);
  }
  lastBtnState = currentBtn;
}
