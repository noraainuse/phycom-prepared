const int thermPin = A0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // 1. อ่านค่า ADC (0-1023) แล้วแปลงเป็นแรงดันไฟ Vout (0-5V)
  int adc = analogRead(thermPin);
  float Vout = adc * (5.0 / 1023.0);

  // จากสมการ Vout = (R / (10000 + R)) * 5
  // vout to r_thermistor
  float R = (330.0 * Vout) / (5.0 - Vout);

  float tempC = (1.0 / ((1.0 / 298.15) + (log(R / 10000.0) / 3950.0))) - 273.15;

  Serial.print("Vout: ");
  Serial.print(Vout);
  Serial.print(" V  |  R: ");
  Serial.print(R);
  Serial.print(" Ohm  |  Temp: ");
  Serial.print(tempC);
  Serial.println(" C");

  delay(1000);
}