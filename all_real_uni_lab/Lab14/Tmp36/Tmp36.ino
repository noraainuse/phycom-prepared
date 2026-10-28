void setup()
{
  Serial.begin(9600);
  pinMode(A0,INPUT);
}

void loop() {
  int raw = analogRead(A0);
  
  float voltage = raw * (5.0 / 1024.0); 
  
  float tempC = (voltage - 0.5) * 100.0;
  Serial.print("Temp: ");
  Serial.print(tempC);
  Serial.println(" C");
  delay(1000);
}