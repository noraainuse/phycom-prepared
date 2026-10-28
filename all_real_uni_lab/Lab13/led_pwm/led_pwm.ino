const int redLed = 3;
const int yellowLed = 5;
const int greenLed = 6;
const int blueLed = 9;

void setup() {
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
}


void loop() {
  for (int i = 0; i <= 255; i++) {
    analogWrite(redLed, i);
    analogWrite(yellowLed, (i * 2) % 256);
    analogWrite(greenLed, 255 - i);
    analogWrite(blueLed, 255 - ((i * 2) % 256));
    delay(20);
  }
}
