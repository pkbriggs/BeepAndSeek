//Transmit test:
//Tests the transmit protocol

#include "messages.h"


int receivingPod;

void setup() {
  Serial.begin(9600);
  receivingPod = 0;
  
}

void loop() {
//  if (Serial.available() > 0) {
//    int incoming = Serial.read();
//    int *data = readData(incoming, NUM_FIELDS, FIELD_SIZES);
//    int podID = data[0];
//    int message = data[1];
//    free(data);
//    
//    switch (message) {
//      case OK:
//        Serial.println("OK");
//        break;
//      case ACTIVATE:
//        Serial.println("ACTIVATE");
//        break;      
//      case FOUND:
//        Serial.println("FOUND");
//        break;      
//      case PANIC:
//        Serial.println("PANIC");
//        break;
//      case OFF:
//        Serial.println("OFF");
//        break;
//      default:
//        break;
//    }
//  }
  
  int nextMessage = OK;
  sendMessage((receivingPod  % 3) + 1, nextMessage);
  receivingPod++;
  delay(1000);
}
