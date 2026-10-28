// โค้ดตัวอย่าง: กดปุ่มส่งค่าเข้าเว็บ + สุ่มตัวเลข (random) ส่งเข้าเว็บผ่าน MQTT
#include <WiFiS3.h>
#include <MQTTClient.h>

const char WIFI_SSID[]      = "68070141";
const char WIFI_PASSWORD[]  = "nano9753";

const char MQTT_BROKER[]    = "phycom.it.kmitl.ac.th";
const int  MQTT_PORT        = 1883; // บอร์ด Arduino ใช้ 1883 เสมอ! (สไลด์หน้า 72)
const char MQTT_CLIENT_ID[] = "UnoR4_68070141_btn";

const int buttonPin = A2; // ต่อปุ่มกดที่ A2 กับ GND
bool lastButtonState = HIGH;
unsigned long lastPublishTime = 0;

WiFiClient network;
MQTTClient mqtt(256);

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
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP); // ไม่กด = HIGH, กด = LOW
  randomSeed(analogRead(A5));       // ทำให้เลขสุ่มไม่ซ้ำเดิมตอนเปิดเครื่องใหม่

  mqtt.begin(MQTT_BROKER, MQTT_PORT, network);
  connectToWiFiAndMQTT();
}

void loop() {
  mqtt.loop();
  if (!mqtt.connected()) {
    connectToWiFiAndMQTT();
  }

  // =========================================================================
  // รูปแบบที่ 1: กดปุ่ม 1 ครั้ง -> ส่งค่า (หรือสุ่มเลข) เข้าเว็บทันทีที่กด!
  // =========================================================================
  bool currentButtonState = digitalRead(buttonPin);
  if (currentButtonState == LOW && lastButtonState == HIGH) { // จังหวะที่เพิ่งกดปุ่มลงไป
    int randNum = random(0, 100); // สุ่มเลขตั้งแต่ 0 ถึง 99 (เลขตัวหลังต้อง +1 เสมอ)

    // ส่งข้อความว่ากดปุ่ม หรือส่งเลขสุ่มขึ้นเว็บ:
    mqtt.publish("68070141/button", "on");
    mqtt.publish("68070141/random", String(randNum));

    Serial.print("Button Pressed! Sent random: ");
    Serial.println(randNum);
    delay(200); // กันปุ่มเด้งซ้ำ (Debounce)
  }
  lastButtonState = currentButtonState;

  // =========================================================================
  // รูปแบบที่ 2: สุ่มค่าส่งเข้าเว็บอัตโนมัติทุกๆ 2 วินาที (เช่น จำลองค่าเซ็นเซอร์)
  // =========================================================================
  if (millis() - lastPublishTime >= 2000) {
    lastPublishTime = millis();

    int fakeTemp = random(25, 36); // สุ่มเลข 25 ถึง 35
    mqtt.publish("68070141/temp", String(fakeTemp));

    Serial.print("Auto sent random temp: ");
    Serial.println(fakeTemp);
  }
}
