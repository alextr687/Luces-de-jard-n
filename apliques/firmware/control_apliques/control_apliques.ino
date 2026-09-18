#define sensor 2
#define luces 3

void setup() {
  pinMode(sensor, INPUT);
  pinMode(luces, OUTPUT);
}

void loop() {
  analogWrite(luces, 100);
  if (digitalRead(sensor)) {
    analogWrite(luces, 255);
    delay(120000);
  }

}
