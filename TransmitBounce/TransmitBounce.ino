//Transmit bounce:
//Counterpart to transmit test - simply bounces the received message back.

#include "messages.h"

int POD_ID = 3;

int ledPin = 11;


void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT); 
}

void loop() {
  if (Serial.available() > 0) {
    int incoming = Serial.read();
//    Serial.println(incoming);
    int *data = readData(incoming, NUM_FIELDS, FIELD_SIZES);
    int podID = data[0];
    int message = data[1];
    free(data);
    
//    Serial.println(podID);
//    Serial.println(message);
    if (podID == POD_ID) {
      switch (message) {
        case OK:
          tone(9, 440, 200);
          Serial.print("OK");
          break;
        case ACTIVATE:
          Serial.print("ACTIVATE");
          break;      
        case FOUND:
          Serial.print("FOUND");
          break;      
        case PANIC:
          Serial.print("PANIC");
          break;
        case OFF:
          Serial.print("OFF");
          break;
        default:
          break;
      }
    }
//    int nextMessage = message; //Bounces the received message back again.
//    sendMessage(sendingPod, nextMessage);
  }
}
