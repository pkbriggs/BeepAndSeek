//POD ID
#define POD_ID 1

#define MAX_PANIC_COUNT 10


// Message protocol
/**
   Fields:
   int:
   [8 bits unused][4bits message][4bits podID]
*/

#define NUM_FIELDS 2         //Each int is split into 2 4-bit sections
int FIELD_SIZES[2] = {4, 4}; //Each field is 4 bits in size.


//Messages:
#define OK 1       //"I'm alive!".
#define ACTIVATE 2 //Hub tells pod to turn on.
#define FOUND 3    //Pod tells hub that it's been pressed.
#define PANIC 4    //Hub tells pod to panic - start blinking and beeping loudly.

//Reserve 13-15 for battery life?
#define LOW_BAT = 13
#define MED_BAT = 14
#define FULL_BAT = 15


#define BITS_PER_BYTE 8


////////////////////////
// Encoding a message //
////////////////////////


/**
  Function: setBits(int& message, int bits, int startPos, int num)
  Modifies message by replacing the bits in message from the startPos (moving to the left by the size of bits)
  with the bit sequence specified by bits.
  Returns: void
*/
void writeData(int& message, int bits, int startPos, int num) {
  for (int i = 0; i < num; i++) {
    if ((startPos + i) >= sizeof(int) * BITS_PER_BYTE) break; //Don't want to write past the end of an int.
    bitWrite(message, startPos + i, bitRead(bits, i));
  }
}
  
///////////////////////     
// Reading a message //
///////////////////////     

/**
  Function: readData(int incomingint, int numFields, int *fieldSizes)
  
  numFields is the number of data fields.
  fieldSizes is a pointer to an array of ints with the number of bits in each data field.
  
  Returns an array containing the bits specified by CHUNKS[] (starting from the right).
  E.g. if the int is 10110111, calling this function with CHUNKS[] = {3, 3, 2} modifies data[]
  to be an array containing 111, 110, and 10. (7, 6, and 2)
  Note that CHUNKS need not use all the bits in the int, but the program will just write zeros past the end of a bitstring.
  
  Returns: int* - a pointer to the array of data created
*/

int* readData(int inInt, int numFields, int *fieldSizes) {
  void *array = malloc(sizeof(int) * numFields);
  int *data = (int *)array;
  for (int i = 0; i < numFields; i++) {
    int chunk = 0;
    for (int j = 0; j < fieldSizes[i]; j++) {
      chunk += bitRead(inInt, 0) * bit(j); //Adds the rightmost bit to the new chunk in the new position.
      inInt = inInt >> 1;           //Shifts the bitstring to the right by 1, moving the next bit into position.
    }
    data[i] = chunk;
  }
  return data;
}


/////////////////////////////////////////
// Specific Functions for our use case //
/////////////////////////////////////////

/**
  Function: sendMessage(int podID, int message)
  Takes the podID and the message that we wish to send, and transmits them via serial.
  Return: void
*/
void sendMessage(int podID, int message) {
  int newMessage = 0;
  writeData(newMessage, podID, 0, 4);
  writeData(newMessage, message, 4, 4);
  Serial.print(newMessage);
}
  
//Example of reading data:
//int *data = readData(inInt, NUM_FIELDS, FIELD_SIZES);
//int podID = data[0];
//int message = data[1];
//free(data);


/**
  Function: sendMessageUntilAcknowledged(int podID, int message, int ntimes)
  Sends a message with a given podID and message ntimes, or until an acknowledgement is received, with a given delay in between.
  Return: bool - whether or not the message was acknowledged
*/
bool sendMessageUntilAcknowledged(int podID, int message, int ntimes, int delayms) {
  int received = -1;
  for (int i = 0; i < ntimes; i++) {
   sendMessage(podID, message);
   delay(delayms);
   if (Serial.available() > 0) {
     received = Serial.read();
     int *data = readData(received, NUM_FIELDS, FIELD_SIZES);
     if (data[1] == OK) return true; //Received an acknowledgement.
   }
  }
  return false;
}








/*
  Global Variables
*/

int panicCounter;

int DELAY_TIME 500

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

// PLAY TONE  ==============================================

/**
  Function: playTone(int tone_, int beat, int duration)
  Plays a tone to the speaker.
  Returns: void
*/
void playTone(int tone_, int beat, int duration, int speakerOut) {
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
void foundSong(int tempo) {
  for (int i=0; i<MAX_COUNT; i++) {
    int tone_ = melody[i];
    int beat = beats[i];

    int duration = beat * tempo; // Set up timing

    playTone(tone_, beat, duration); 
    // A pause between notes...
    delayMicroseconds(pause);
  }
}

// Plays Panic alarm when puck is not found
void playPanicSong(int tempo) {
  for (int i=0; i<3; i++) {
    int tone_ = melody[i];
    int beat = panicBeats[i];

    int duration = beat * tempo; // Set up timing

    playTone(tone_, beat, duration); 
    // A pause between notes...
    delayMicroseconds(pause);
  }
}

void onMode() {
  while (true) {
    blinkTime(DELAY_TIME);
    beep();
    digitalRead(buttonPin);
    if (buttonState != 1) break; //if button is pressed
  }
  foundSong(); //pressed the button. Found the puck!
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
      panic();
    }
    blink();
  }
}



