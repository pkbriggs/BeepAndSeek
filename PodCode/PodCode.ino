#include <Messages.h>
#include "pitches.h"
/*
 Pod Code
 Code that will end up going into the pods for the game BeepAndSeek
*/


/*
  Global Variables
*/

//ID for this pod (CHANGE WHEN UPLOADING CODE TO DIFFERENT PODS)
int POD_ID = 1;

//Speaker melodies:
int marioNotes[] = {NOTE_E5, NOTE_E5, NOTE_E5, NOTE_C5, NOTE_E5, NOTE_G5};
int marioNotesLength = 6;
int marioBeats[] = {16, 8, 8, 16, 8, 4, 4};

//Counts the number of loops that have gone by without a signal from the hub.
int panicCounter;

//Pins for IO objects
int ledPin = 11;
int buttonPin = 10;
int speakerPin = 9;


/*
  Function: playSong(int speakerPin, int *melody, int melodyLength, int *beats)
  Plays the specified song (which is specified fully with a melody array, a beats
  array, and the number of notes in the melody (melodylength)). 
  Copied mostly from http://arduino.cc/en/Tutorial/tone
*/
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

/*
  Function: onMode()
  Contains the main loop that runs when the pod is activated and waiting for
  its button to be pressed.
  The pod maintains a lit LED and reads the serial port for incoming messages
  from the pod that let it know that it's still connected to the hub. 
  When the button is pressed (checked with the proper debouncing) the loop
  breaks the the function returns with a song.
  Returns: void  
*/

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
        //Reset panic counter:
        panicCounter = 0;  
        break;
      case OFF:
        return; //Stop being on.
      case PANIC:
        panic();
        break;
    }
    digitalWrite(ledPin, HIGH); //Turn the LED on
    //Beep:
    tone(speakerPin, 440, 100);
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
  pinMode(speakerPin, OUTPUT);
  
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



