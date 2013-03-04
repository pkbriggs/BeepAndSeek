//
//  Messages.cpp
//  
//
//  Created by Mark Nishimura on 3/1/13.
//
//

#include "Messages.h"
//////////////////////
// Message protocol //
//////////////////////





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
 
 Returns an array containing the bits specified by fieldSizes[] (starting from the right).
 E.g. if the int is 10110111, calling this function with fieldSizes[] = {3, 3, 2} modifies data[]
 to be an array containing 111, 110, and 10. (7, 6, and 2)
 Note that fieldSizes need not use all the bits in the int, but the program will just write zeros past the end of a bitstring.
 
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

