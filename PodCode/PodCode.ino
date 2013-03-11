//#include <Messages.h>
#include "pitches.h"
#include "messages2.h"
/*
 Pod Code
 Code that will end up going into the pods for the game BeepAndSeek
 */


/*
  Global Variables
 */

//ID for this pod (CHANGE WHEN UPLOADING CODE TO DIFFERENT PODS)
int POD_ID = 7;

//Number of times to wait before panicking:
int MAX_PANIC_COUNT = 20;

//Speaker melodies:
int marioNotes[] = {NOTE_C4, NOTE_F4};
int marioNotesLength = 20;
int marioBeats[] = {4, 4};

//Counts the number of loops that have gone by without a signal from the hub.
int panicCounter;

//Flag that tells whether the pod has previously been activated and then found.
bool alreadyActivated;

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
  }
}


/*
  Function: panic()
 Pod panics, blinking its led and beeping rapidly.
 Return: void
 */

void panic() {
  while (true) {
    if (Serial.available() > 0) {
      int incoming = Serial.read();
      //int *data = readData(incoming, NUM_FIELDS, FIELD_SIZES); //NUM_FIELDS and FIELD_SIZES from Messages.h
      readData(incoming);
      int podID = data[0];
      int message = data[1];
      if (podID == POD_ID) {
        panicCounter = 0;
        return; //We got a message
      }
    }
//    tone(speakerPin, 440, 20);
    digitalWrite(ledPin, HIGH);
    delay(50);
    digitalWrite(ledPin, LOW);
    delay(50);
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
//  int lastButtonState = HIGH;
  int DEBOUNCE_TIME = 100; //ms
  int lastDebounceTime = millis();

  while (true) {
    if (Serial.available() > 0) {
      int incoming = Serial.read();
      readData(incoming);
      int podID = data[0];
      int message = data[1];
      Serial.flush();
      //Debugging:
//      Serial.println(incoming);
      
      switch (message) {
        case OK:
          sendMessage(POD_ID, OK);
          //Reset panic counter:
          panicCounter = 0;  
          break;
        case RESET:
          sendMessage(POD_ID, OK);
          alreadyActivated = false;
          return; //Stop being on.
        case PANIC:
          panic();
          break;
      }
    }
    digitalWrite(ledPin, HIGH); //Turn the LED on
    //Beep:
    tone(speakerPin, NOTE_E6, 20);
    //Debounce testing:
    int reading = digitalRead(buttonPin);
    if ((reading == LOW) && ((millis() - lastDebounceTime) > DEBOUNCE_TIME))
    {
      break;
    }
    else
    {
      lastDebounceTime = millis();
    }
    delay(100);
  }
//  playSong(speakerPin, marioNotes, marioNotesLength, marioBeats); //pressed the button. Found the puck!

}



void setup()
{
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
  pinMode(speakerPin, OUTPUT);

  panicCounter = 0;
  alreadyActivated = false;
}

void loop()
{
  if (Serial.available() > 0) {
    int incoming = Serial.read();
    readData(incoming);
    int podID = data[0];
    int message = data[1];

    if (podID == POD_ID) {
      tone(speakerPin, 440, 100);
      switch (message) {
        case OK:     //Request for acknowledgement
//          Serial.print("Sending OK message for pod #");
//          Serial.println(podID);
          sendMessage(POD_ID, OK); 
          break;
        case ACTIVATE:
          if (!alreadyActivated) {
            alreadyActivated = true;
            sendMessageUntilAcknowledged(POD_ID, OK, 5, 30);
            //Activate!
            onMode();
            //Send word that I've been pressed!
            sendMessageUntilAcknowledged(POD_ID, FOUND, 10, 30); //Send it 10 times.
          }
          break;
        case RESET:
          sendMessage(POD_ID, OK);
          alreadyActivated = false;
          break; //Stop being on.
        case PANIC:
          panic();
          break;
        default:
          break;
      }
      Serial.flush(); //Flush any remaining  messages from the buffer

    }
    panicCounter = 0; //Reset panic counter because we received a message,
                      //even if it wasn't for us.
  }
//  if (panicCounter < MAX_PANIC_COUNT) {
//    panicCounter++;
//  }
//  else if (panicCounter == MAX_PANIC_COUNT) {
//    panic();
//  }
  digitalWrite(ledPin, HIGH);
  /*delay(500);
  digitalWrite(ledPin, LOW);
  delay(500);*/
//  delay(250); //wait
}





