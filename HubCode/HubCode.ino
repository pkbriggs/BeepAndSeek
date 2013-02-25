/**
  HubCode
*/

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
  Serial.write(newMessage);
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




/**
  Global variables
*/
bool needToActivatePod;
int podToActivate;
int podToPing;



void setup() {
  Serial.begin(9600);
  
  bool needToActivatePod = true;
  int podToActivate = 1;
  int podToPing = 1;
}



void loop() {
  //Check if anything is in the buffer to be read:
  if (Serial.available() > 0) {
    int incoming = Serial.read();
    int *data = readData(incoming, NUM_FIELDS, FIELD_SIZES);
    int podID = data[0];
    int message = data[1];
    free(data);
    if (message == FOUND) {
      //Pod was found, activate next pod.
      needToActivatePod = true;
      podToActivate = podID + 1;
    }
  }
  
  //If we need to activate a pod, do it:

  if (needToActivatePod) {
    sendMessage(podToActivate, ACTIVATE);
    needToActivatePod = false; //Reset flag
  }
  
  //Ping the next pod we need to ping to ensure that it's still alive.
  bool podIsAlive = sendMessageUntilAcknowledged(podToPing, OK, 10, 100);
  if (!podIsAlive) {
    //Panic!
    sendMessageUntilAcknowledged(podToPing, PANIC, 10, 100);
    //This basically just sends the message 10 times. If the pod doesn't acknowledge, hopefully at some point, the pod's own code will kick in and start complaining.
  }
}


