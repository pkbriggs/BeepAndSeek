//
//  Messages.h
//  
//
//  Created by Mark Nishimura on 3/1/13.
//
//

#ifndef ____Messages__
#define ____Messages__

//////////////////////
// Message protocol //
//////////////////////

#ifndef __Messages.h
#define __messages.h


#include "HardwareSerial.h"
#include "binary.h"

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
#define OFF 5      //Hub tells pod to turn off, wait for further signals.

//Reserve 13-15 for battery life
#define LOW_BAT = 13
#define MED_BAT = 14
#define FULL_BAT = 15

//Used for encoding messages
#define BITS_PER_BYTE 8

//Used for deciding whether or not to enter panic mode
#define MAX_PANIC_COUNT 10


////////////////////////
// Encoding a message //
////////////////////////


/**
 Function: setBits(int& message, int bits, int startPos, int num)
 Modifies message by replacing the bits in message from the startPos (moving to the left by the size of bits)
 with the bit sequence specified by bits.
 Returns: void
 */
void writeData(int& message, int bits, int startPos, int num);

///////////////////////
// Reading a message //
///////////////////////

/**
 Function: readData(int incomingint, int numFields, int *fieldSizes)
 
 numFields is the number of data fields.
 fieldSizes is a pointer to an array of ints with the number of bits in each data field.
 
 Returns an array containing the bits specified by fieldSizes[] (starting from the right).
 E.g. if the int is 10110111, calling this function with fieldSizes[] = {3, 3, 2} modifies data[]
 to be an array containing 111, 110, and 10. (7, 6, and 2)
 Note that fieldSizes need not use all the bits in the int, but the program will just write zeros past the end of a bitstring.
 
 Returns: int* - a pointer to the array of data created
 */

int* readData(int inInt, int numFields, int *fieldSizes);

/////////////////////////////////////////
// Specific Functions for our use case //
/////////////////////////////////////////

/**
 Function: sendMessage(int podID, int message)
 Takes the podID and the message that we wish to send, and transmits them via serial.
 Return: void
 */
void sendMessage(int podID, int message);

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
bool sendMessageUntilAcknowledged(int podID, int message, int ntimes, int delayms);


#endif

#endif /* defined(____Messages__) */
