//Test Pod Button

int ledPin = 11;
int buttonPin = 10;

int buttonState;
int lastButtonState = LOW;

long lastDebounceTime = 0;
long debounceDelay = 1000;

//bool activated = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  
}

void loop() {
  while (true) {
    
    //Wait for button press:
    int reading = digitalRead(buttonPin);
    if (reading != LOW) {
      lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
      break;
    }    
    
  }
  while (true) {
    //Blink
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
    
    //Wait for button press:
    int reading = digitalRead(buttonPin);
    if (reading != LOW) {
      lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
      break;
    }
  }
  while (true) {
    digitalWrite(ledPin, HIGH); //for eternity
  }
}
  
  
  
