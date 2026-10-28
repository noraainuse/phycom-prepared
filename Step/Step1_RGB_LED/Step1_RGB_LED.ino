int redPin= 9;
int greenPin = 10;
int bluePin = 11;
int buttonPin = A2;
unsigned long lastTime = 0;
int step = 0;

void setup() {
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);              
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}
void loop() 
  if (digitalRead(buttonPin) == LOW) {
    setColor(255, 0, 255);
    delay(50);
  }
  if (millis() - lastTime >= 1000) {
      lastTime = millis();
      if (step == 0) setColor(255, 0, 0);
      else if (step == 1) setColor(0, 255, 0);
      else if (step == 2) setColor(0, 0, 255);
      step = (step + 1) % 3;
    }
  
}

void setColor(int red, int green, int blue) {
  analogWrite(redPin, 255 - red);
  analogWrite(greenPin, 255 - green);
  analogWrite(bluePin, 255 - blue);
}
