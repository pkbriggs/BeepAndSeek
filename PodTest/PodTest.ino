//Test Pod Button

int ledPin = 11;
int buttonPin = 10;

//bool activated = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
}

void loop() {
  while (!activated) {
    if (digitalRead(buttonPin, HIGH)) break;
  }
  while (true) {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
    if (digitalRead(buttonPin, HIGH)) break;
  }
  while (true) {
    digitalWrite(ledPin, HIGH); //for eternity
  }
}
  
  
  
