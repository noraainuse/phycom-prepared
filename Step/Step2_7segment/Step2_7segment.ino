const int segPins[7] = {7, 6, 4, 2, 1, 9, 10}; // a, b, c, d, e, f, g
const int num_array[10][7] = {
  // a, b, c, d, e, f, g
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};
unsigned long lastTime = 0;
int currentDigit = 0;

void setup() {
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void displayDigit(int digit) {
  digitalWrite(7, num_array[digit][0]);
  digitalWrite(6, num_array[digit][1]);
  digitalWrite(4, num_array[digit][2]);
  digitalWrite(2, num_array[digit][3]);
  digitalWrite(1, num_array[digit][4]);
  digitalWrite(9, num_array[digit][5]);
  digitalWrite(10, num_array[digit][6]);
}
 
void loop() {
  if (millis() - lastTime >= 1000) {
    lastTime = millis();
    displayDigit(currentDigit);             // แสดงเลขปัจจุบัน
    currentDigit = (currentDigit + 1) % 10; // ขยับไปเลขถัดไป (พอถึง 10 จะหารเอาเศษกลับมาเป็น 0)
  }
}