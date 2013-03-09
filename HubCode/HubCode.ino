//#include <Messages.h>
#include "messages.h"

/**
 * HubCode
 */

//Debugging:
int ledPin = 11;
int speakerPin = 9;

/**
 * Global variables
 */
bool needToActivatePod;
int podToActivate;
int activatedPod;
int podToPing;

unsigned long startTime;

int numPods;


/*
  Function: resetGame(int numPods)
 
 */

//void resetPods(int numPods) {
//  for (int i = 0; i < numPods; i++) {
//    sendMessage(




void setup() {
//  digitalWrite(ledPin, HIGH);
  Serial.begin(9600);

  needToActivatePod = true;
  podToActivate = 1;
  activatedPod = -1; //No activatedPod yet.
  podToPing = 1;

  startTime = millis(); 


  numPods = 2;
}



void loop() {
//  digitalWrite(ledPin, HIGH);
  //Check if anything is in the buffer to be read:
  if (Serial.available() > 0) {
    int incoming = Serial.read();
    int *data = readData(incoming, NUM_FIELDS, FIELD_SIZES);
    int podID = data[0];
    int message = data[1];
    free(data);
    Serial.flush();
    
//    Serial.print(incoming);
    
    if (message == FOUND && podID == activatedPod) {
      //If this is the last pod, exit the game.
      if (podID >= numPods) {
        float time = millis() - startTime / 1000.0;

        while (true) {
          Serial.println("Time:");
          Serial.println(time);
          tone(speakerPin, 440, 250);
          delay(2000);
        } //Stop and wait for reset.
      }
      
      else {
        //Pod was found, activate next pod.
        needToActivatePod = true;
        podToActivate = podID + 1;
      }
    }
    else {
      tone(speakerPin, 440, 500);
      delay(1000);
      Serial.print(incoming);
    }
  }

  //If we need to activate a pod, do it:

  if (needToActivatePod) {
    //Keep sending messages until the pod responds.
    int panicCounter = 0;
    digitalWrite(ledPin, HIGH);
    sendMessageUntilAcknowledged(podToActivate, ACTIVATE, -1, 1000);
//    while(!sendMessageUntilAcknowledged(podToActivate, ACTIVATE, -1, 100)) {
//      if (panicCounter < 10) {
//        panicCounter++;
//      }
//      else if (panicCounter == 10) {
//        sendMessageUntilAcknowledged(podToActivate, PANIC, 10, 100);
//      }
//    }
    digitalWrite(ledPin, LOW);
    needToActivatePod = false; //Reset flag
    activatedPod = podToActivate;
  }

  //Ping the next pod we need to ping to ensure that it's still alive.
//  bool podIsAlive = sendMessageUntilAcknowledged(podToPing, OK, -1, 100);
//  if (!podIsAlive) {
//    //Panic!
//    sendMessageUntilAcknowledged(podToPing, PANIC, 10, 100);
//    //This basically just sends the message 10 times. If the pod doesn't acknowledge, hopefully at some point, the pod's own code will kick in and start complaining.
//  }
}



