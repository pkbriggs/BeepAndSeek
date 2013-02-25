// LED constant
const int ledPin = 13; // the pin that the LED is attached to

// XBee data
int messages[] = {0, 1, 2, 3};
int received[] = {};
int panicCounter = 0;
boolean isFound = false;

// button constants
const int buttonPin = 2; //2 if unpressed?
int buttonState = 0；

int puckState = 0; //0 means idle, 1 means active

// Pizo constants
int speakerOut = 0;
#define  c     3830    // 261 Hz 
#define  d     3400    // 294 Hz 
#define  e     3038    // 329 Hz 
#define  f     2864    // 349 Hz 
#define  g     2550    // 392 Hz 
#define  a     2272    // 440 Hz 
#define  b     2028    // 493 Hz 
#define  C     1912    // 523 Hz 
// Define a special note, 'R', to represent a rest
#define  R     0


//Messages:
#define OK 1       //"I'm alive!"
#define ACTIVATE 2 //Hub tells pod to turn on
#define FOUND 3    //Pod tells hub that it's been pressed.






// For the blinking when the module is the one currently searched for.
void blink() {
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  delay(500);
}

// MELODY and TIMING  =======================================
//  melody[] is an array of notes, accompanied by beats[], 
//  which sets each note's relative length (higher #, longer note) 
int melody[] = {  C,  b,  g,  C,  b,   e,  R,  C,  c,  g, a, C };
int beats[]  = { 16, 16, 16,  8,  8,  16, 32, 16, 16, 16, 8, 8 }; 
int idleBeeps[] = {e, R, e, R, e, R, e}
int idleBeats[] = {2,1,2,1,2,1,2};
int panicBeats[] = {4,4,4};

int MAX_COUNT = sizeof(melody) / 2; // Melody length, for looping.
int IDLE_LENGTH = 7;
int PANIC_LENGTH = 3;

// Set overall tempo
long tempo = 10000;
// Set length of pause between notes
int pause = 1000;
// Loop variable to increase Rest length
int rest_count = 100; //<-BLETCHEROUS HACK; See NOTES

// Initialize core variables
int tone_ = 0;
int beat = 0;
long duration  = 0;

// PLAY TONE  ==============================================

// Pulse the speaker to play a tone for a particular duration
void playTone() {
  long elapsed_time = 0;
  if (tone_ > 0) { // if this isn't a Rest beat, while the tone has 
    //  played less long than 'duration', pulse speaker HIGH and LOW
    while (elapsed_time < duration) {
      digitalWrite(speakerOut,HIGH);
      delayMicroseconds(tone_ / 2);

      // DOWN
      digitalWrite(speakerOut, LOW);
      delayMicroseconds(tone_ / 2);

      // Keep track of how long we pulsed
      elapsed_time += (tone_);
    } 
  }
  else { // Rest beat; loop times delay
    for (int j = 0; j < rest_count; j++) { // See NOTE on rest_count
      delayMicroseconds(duration);  
    }                                
  }                                 
}

// Plays when button is pressed.
void foundSong() {
  for (int i=0; i<MAX_COUNT; i++) {
    tone_ = melody[i];
    beat = beats[i];

    duration = beat * tempo; // Set up timing

    playTone(); 
    // A pause between notes...
    delayMicroseconds(pause);
  }
}

// Plays Panic alarm when puck is not found
void playPanicSong() {
  for (int i=0; i<3; i++) {
    tone_ = melody[i];
    beat = panicBeats[i];

    duration = beat * tempo; // Set up timing

    playTone(); 
    // A pause between notes...
    delayMicroseconds(pause);
  }
}

void beep()) {
  for (int i=0; i<IDLE_LENGTH; i++) {
    tone_ = idleBeeps[i];
    beat = idleBeats[i];

    duration = beat * tempo; // Set up timing

    playTone(); 
    // A pause between notes...
    delayMicroseconds(pause);
  }
}

// Module is activated and ready to be found.
void onMode() {
  while (puckState == 1) {
    blink();
    beep();
    digitalRead(buttonPin);
    if (buttonState != 1)break; //if button is pressed
  }
  foundSong(); //pressed the button. Found the puck!
}

void panicMode() {
  playPanicSong();
}

void msgNextPuck() {

}

#define MAX_PANIC_STEPS 100

// Message protocol

/**
   Upper 4 bits are the message
   Lower 4 bits are the ID of relevant node.
*/

int setPodID(int podNum) {
  if (podNum > 15) { //max number for 4 bits. 
    error("Pod number out of range!")
  }
  return podNum;
}

int setMessage(int message, int lbits) {
  if (message > 15) {
    error("Message too large!");
  }
  return lbits + message*bit(4); //Sets the message to be the upper 4 bits.
}

int readPodID(int incomingByte) {
  int id = 0;
  for (int i = 0; i < 4; i++) {
    id += incomingByte.readBit(i);
  }
  return id;
}

int readMessage(int incomingByte) {
  int message = 0;
  for (int i = 4; i < 8; i++) {
    message += incomingByte.readBit(i);
  }
  return message;
}



void setup()
{
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(speakerOut, OUTPUT);
}





void loop()
{
  while(panicCounter > MAX_PANIC_STEPS) { //OUT OF RANGE
   panicMode(); 
   if (Serial.available() > 0) { //if signal found, stop panic and reset counter
     panicCounter = 0;
     break;
   }
  }
  if (Serial.available() > 0) { //this check is redundant
    int incoming = Serial.read();
    
    int podID = readPodID(incoming);
    
    if (podID != 
    
    if (incoming == 'A') { //'A' for activated. This puck is next to be found
      puckState = 1;
     onMode();
     //puck is found, move on to next one
     //call next puck; def msgNextPuck(); send an 'A'
    }
    else {
      puckState = 0; //still idle
      blink();
    }

    /*else if (!isFound) { //idleMode
     digitalWrite(ledPin, HIGH);
    }
    else digitalWrite(ledPin, LOW);*/
  }
  else panicCounter++; 
}


//Changelog: not sure what the !isFound boolean is for. The puck will exit out of onMode and message the next puck when its button is pressed. Also, the boolean was out of scope. 

//Added a panic mode song and panic mode. 
//Also, I'm not sure what to do with the button variables buttonState and buttonPin (what are the values when pressed/unpressed). 
//Otherwise, we just need to write a method to ping the next puck. 
//Should we have a puck identity number so that a puck knows that it's the last puck?
//
//Cheers!
