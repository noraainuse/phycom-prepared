# สารบัญไฟล์ `.ino` ทั้งหมดในโฟลเดอร์ `mqttsmth/` (พร้อมเปิดสอบทันที)

ทุกไฟล์ตั้งค่า WiFi (`68070141` / `nano9753`), Broker (`phycom.it.kmitl.ac.th`), และ Port (`1883`) ไว้ให้เรียบร้อยแล้ว เจอโจทย์ข้อไหนในห้องสอบ คลิกเปิดไฟล์นั้นแล้วกด Upload ได้เลย!

---

## หมวดที่ 1: แยกไฟล์เดี่ยวตามข้อสอบ Mock Exam เป๊ะๆ (6 ข้อ)

| ข้อสอบ | ชื่อไฟล์ `.ino` | รายละเอียดการทำงาน & พินที่ต่อ |
| :--- | :--- | :--- |
| **ข้อ 1A (Publish Temp 3 ขา)** | [Exam1A_Pub_Temp_TMP36.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Exam1A_Pub_Temp_TMP36/Exam1A_Pub_Temp_TMP36.ino) | อ่านอุณหภูมิจาก **TMP36 / MCP9700 (`A0`)** ส่งขึ้น `68070141/temp` |
| **ข้อ 1B (Publish Temp 2 ขา)** | [Exam1B_Pub_Temp_Thermistor.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Exam1B_Pub_Temp_Thermistor/Exam1B_Pub_Temp_Thermistor.ino) | อ่านอุณหภูมิจาก **Thermistor + R (`A0`)** ส่งขึ้น `68070141/temp` |
| **ข้อ 2 (Publish Light/Pot)** | [Exam2_Pub_Light_Potentiometer.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Exam2_Pub_Light_Potentiometer/Exam2_Pub_Light_Potentiometer.ino) | อ่านค่า **Potentiometer / LDR (`A1`)** ค่า `0–1023` ส่งขึ้น `68070141/light` |
| **ข้อ 3 (Publish Food)** | [Exam3_Pub_Food_Ultrasonic.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Exam3_Pub_Food_Ultrasonic/Exam3_Pub_Food_Ultrasonic.ino) | อ่านระยะ **Ultrasonic (`D12, D13`)**: `> 20 cm` ส่ง `"off"`, `<= 20 cm` ส่ง `"on"` ขึ้น `68070141/food` |
| **ข้อ 4 (Publish Door)** | [Exam4_Pub_Door_Ultra_Button.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Exam4_Pub_Door_Ultra_Button/Exam4_Pub_Door_Ultra_Button.ino) | **Ultrasonic (`D12, D13`) + ปุ่มกด (`A2`)**: `>= 20 cm && กดปุ่ม` ส่ง `"On"`, อื่นๆ ส่ง `"Off"` ขึ้น `68070141/door` |
| **ข้อ 5 (Subscribe Venus)** | [Exam5_Sub_Venus_RGB_LED.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Exam5_Sub_Venus_RGB_LED/Exam5_Sub_Venus_RGB_LED.ino) | รับค่าจาก `68070141/venus` แสดงสี **RGB LED (`D9, D10, D11`)**: `36–50` แดง, `26–35` ฟ้า/น้ำเงิน, `10–25` เขียว |
| **ข้อ 6 (Subscribe 7-Seg)** | [Exam6_Sub_ProbStat_7Segment.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Exam6_Sub_ProbStat_7Segment/Exam6_Sub_ProbStat_7Segment.ino) | รับเลข `0–9` จาก `68070141/prob_stat` แสดงบน **7-Segment 1 หลัก (`D2..D8`)** |

---

## หมวดที่ 2: โจทย์ประยุกต์จับคู่ 2 อุปกรณ์มารวมกัน (Combinations)

| รหัส | ชื่อไฟล์ `.ino` | การจับคู่อุปกรณ์ (Combination) & เงื่อนไขประยุกต์ |
| :--- | :--- | :--- |
| **Combo 01** | [Combo01_Temp_And_Button_Pub.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Combo01_Temp_And_Button_Pub/Combo01_Temp_And_Button_Pub.ino) | **Temp (`A0`) + ปุ่มกด (`A2`)**: เช็ค `temp >= 30 && กดปุ่ม` ส่ง `"On"`, หรือกดปุ่มเพื่อส่งค่า/สลับหน่วย C กับ F |
| **Combo 02** | [Combo02_Pot_And_Button_Pub.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Combo02_Pot_And_Button_Pub/Combo02_Pot_And_Button_Pub.ino) | **Potentiometer/LDR (`A1`) + ปุ่มกด (`A2`)**: เช็คค่าแสง `>= 500 && กดปุ่ม` ส่ง `"On"` พร้อมส่งค่า `0–1023` |
| **Combo 03** | [Combo03_Temp_To_RGB_And_Pub.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Combo03_Temp_To_RGB_And_Pub/Combo03_Temp_To_RGB_And_Pub.ino) | **Temp (`A0`) + RGB LED (`9,10,11`)**: วัดอุณหภูมิจริงแล้วเปลี่ยนสีไฟ RGB บนบอร์ดทันที (ร้อน=แดง, เย็น=เขียว) + ส่ง MQTT |
| **Combo 04** | [Combo04_Temp_To_7Seg_And_Pub.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Combo04_Temp_To_7Seg_And_Pub/Combo04_Temp_To_7Seg_And_Pub.ino) | **Temp (`A0`) + 7-Segment (`2..8`)**: วัดอุณหภูมิแล้วเอาเลขหลักหน่วยไปโชว์บน 7-Segment + ส่ง MQTT |
| **Combo 05** | [Combo05_Ultrasonic_To_RGB_And_Pub.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Combo05_Ultrasonic_To_RGB_And_Pub/Combo05_Ultrasonic_To_RGB_And_Pub.ino) | **Ultrasonic (`12,13`) + RGB LED (`9,10,11`)**: เตือนระยะใกล้/กลาง/ไกลด้วยสี แดง/เขียว/น้ำเงิน + ส่ง MQTT |
| **Combo 06** | [Combo06_Ultrasonic_To_7Seg_And_Pub.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Combo06_Ultrasonic_To_7Seg_And_Pub/Combo06_Ultrasonic_To_7Seg_And_Pub.ino) | **Ultrasonic (`12,13`) + 7-Segment (`2..8`)**: แปลงระยะทาง cm เป็นตัวเลขระดับ `0–9` บน 7-Segment + ส่ง MQTT |
| **Combo 07** | [Combo07_Pot_To_PWM_LED_And_7Seg.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Combo07_Pot_To_PWM_LED_And_7Seg/Combo07_Pot_To_PWM_LED_And_7Seg.ino) | **Potentiometer (`A1`) + หรี่ไฟ PWM (`9`) + 7-Segment (`2..8`)**: หมุนปรับความสว่างไฟ `0–255` + โชว์ระดับ `0–9` + ส่ง MQTT |
| **Combo 08** | [Combo08_Button_3Switches_To_RGB.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Combo08_Button_3Switches_To_RGB/Combo08_Button_3Switches_To_RGB.ino) | **ปุ่มกด (`A2`) + RGB LED (`9,10,11`)**: กดปุ่มสลับสี Red/Green/Blue/Purple/Off พร้อมส่งชื่อสีขึ้น MQTT และรับชื่อสีจากเว็บได้ |
| **Combo 09** | [Combo09_Sub_To_2Digit_7Segment.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Combo09_Sub_To_2Digit_7Segment/Combo09_Sub_To_2Digit_7Segment.ino) | **MQTT Subscribe + 7-Segment 2 หลัก**: รับค่าตัวเลข 2 หลัก (`00–99`) จากเว็บมาแสดงบนจอ 7-Segment 2 หลัก |
| **Combo 10** | [Combo10_Sub_And_Sensor_To_LCD_I2C.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/Combo10_Sub_And_Sensor_To_LCD_I2C/Combo10_Sub_And_Sensor_To_LCD_I2C.ino) | **Temp (`A0`) + จอ I2C LCD 16x2 (`A4,A5`) + MQTT**: บรรทัดบนโชว์ค่าเซ็นเซอร์บอร์ด บรรทัดล่างโชว์ค่าที่รับมาจาก Subscribe |

---

## หมวดที่ 3: ไฟล์รวมทุกข้อ & ไฟล์ตัวช่วยทดสอบ

* [03_MQTT_All_In_One_Exam.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/03_MQTT_All_In_One_Exam/03_MQTT_All_In_One_Exam.ino) — **รวมครบทั้ง 6 ข้อในไฟล์เดียว** (ถ้าต่อทุกอย่างไว้บนบอร์ดเดียว อัปโหลดไฟล์นี้ทีเดียวผ่านทุกข้อ!)
* [06_MQTT_Button_And_Random.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/06_MQTT_Button_And_Random/06_MQTT_Button_And_Random.ino) — กดปุ่มส่งค่าเข้าเว็บ + สุ่มค่า (`random`) ส่งเข้าเว็บอัตโนมัติ (ใช้เทสต์ว่าส่งขึ้นหน้าเว็บผ่านไหมโดยไม่ต้องต่อเซ็นเซอร์)
* [01_Ultrasonic_Button_Step4.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/01_Ultrasonic_Button_Step4/01_Ultrasonic_Button_Step4.ino) — ทดสอบ Ultrasonic + ปุ่มกดแบบออฟไลน์
* [02_All_Sensors_And_Lights_Test.ino](file:///home/lynn/codex/Uniwork/phycom-prepared/mqttsmth/02_All_Sensors_And_Lights_Test/02_All_Sensors_And_Lights_Test.ino) — ทดสอบสายไฟทุกพินแบบออฟไลน์ก่อนสอบ
