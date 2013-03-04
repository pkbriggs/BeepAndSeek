//#include <Messages.h>
#include "messages.h"

/**
  HubCode
*/

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


