#include "pitches.h"
#include "messages.h"

//Speaker melodies:
int marioNotes[] = {NOTE_E5, NOTE_E5, NOTE_E5, NOTE_C5, NOTE_E5, NOTE_G5};
int marioNotesLength = 6;
int marioBeats[] = {16, 8, 8, 16, 8, 4, 4}

/*
  Global Variables
*/

int panicCounter;

int DELAY_TIME 500;

int ledPin;
int buttonPin;
int speakerOut;

/**
  Function: blinkTime(int delayms, int ledPin)
  Turns an LED on and off for a given delay time.
  Returns: void
*/

void blinkTime(int delayms, int ledPin) {
  digitalWrite(ledPin, HIGH);
  delay(delayms);
  digitalWrite(ledPin, LOW);
  delay(delayms);
}


// Plays when button is pressed.
void playSong(int speakerPin, int *melody, int melodyLength, int *beats) {
  for (int i=0; i < melodyLength; i++) {
    int tone_ = melody[i];
    int beat = beats[i];

    int duration = 1000 / beat; 

    tone(speakerPin, tone_, duration);
    // A pause between notes...
    int pauseBetweenNotes = duration * 1.30;
    delay(pauseBetweenNotes);
    noTone(speakerPin);
  }
}


void onMode() {
  int buttonState = digitalRead(buttonPin);
  int reading;
  int DEBOUNCE_TIME = 50; //ms
  int lastDebounceTime = millis();
  
  while (true) {
    if (Serial.available() > 0) {
      int incoming = Serial.read();
      int *data = readData(incoming, NUM_FIELDS, FIELD_SIZES);
      int podID = data[0];
      int message = data[1];
      free(data);
    }
    switch (message) {
      case OK:
        sendMessage(POD_ID, OK); 
        break;
      case OFF:
        return; //Stop being on.
      case PANIC:
        panic();
        break;
    }
//    blinkTime(DELAY_TIME);
    digitalWrite(ledPin, HIGH); //Turn the LED on
    //Beep:
    tone(speakerPin, 440, 250);
    //Debounce testing:
    reading = digitalRead(buttonPin);
    if (reading != lastButtonState) lastDebounceTime = millis();
    if ((millis() - lastDebounceTime) > DEBOUNCE_TIME) break; //Button was pressed, leave the loop.
  }
  playSong(speakerPin, marioNotes, marioNotesLength, marioBeats); //pressed the button. Found the puck!
}


void setup()
{
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(speakerOut, OUTPUT);
  
  panicCounter = 0;
}

void loop()
{
  if (Serial.available() > 0) {
    int incoming = Serial.read();
    int *data = readData(incoming, NUM_FIELDS, FIELD_SIZES);
    int podID = data[0];
    int message = data[1];
    delete[] data;
    
    if (podID == POD_ID) {
      switch (message) {
        case OK:     //Request for acknowledgement
          sendMessage(POD_ID, OK); 
          break;
        case ACTIVATE:
          //Activate!
          onMode();
          //Send word that I've been pressed!
          sendMessage(POD_ID, FOUND);
          break;
        default:
          if (panicCounter < MAX_PANIC_COUNT) {
            panicCounter++;
          }
          break;
      }
    }
    
    if (panicCounter == MAX_PANIC_COUNT) {
      panicMode();
    }
    blink();
  }
}



