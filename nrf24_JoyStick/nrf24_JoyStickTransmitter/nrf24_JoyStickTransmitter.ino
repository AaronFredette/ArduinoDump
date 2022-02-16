
/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"
#include <printf.h>
/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 1;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(10,8);
/**********************************************************/

byte addresses[][6] = {"1Node","2Node"};

int controllerData[4] = {5,89,1,0}; // x cord, y cord, laser instruct, win scenario

// Used to control whether this node is sending or receiving // 1= transmit,  0 = receive
bool role = 1;

void setup() {
  Serial.begin(115200);
  printf_begin();
  
  Serial.println(radio.begin());
  radio.setAutoAck(false);
  radio.setRetries(15,15);
  radio.setChannel(78);
  Serial.println(F("RF24/examples/GettingStarted"));
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));

  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_MAX);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
  
  // Start the radio listening for data
  radio.startListening();

  radio.printDetails();
}

void loop() {
  
    radio.stopListening();                                    // First, stop listening so we can talk.
    
    unsigned long start_time = micros();                             // Take the time, and send it.  This will block until complete
    Serial.println(F("Now sending: "));
    Serial.print("x cord : ");
    Serial.println(controllerData[0]);
    Serial.print("y cord : ");
    Serial.println(controllerData[1]);
    Serial.print("laser command : ");
    Serial.println(controllerData[2]);
    Serial.print("win scenario : ");
    Serial.println(controllerData[3]);

    
     if (!radio.write( &controllerData, sizeof(controllerData) )){
       Serial.println(F("failed to send"));
     }
        
    radio.startListening();                                    // Now, continue listening
    
    unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
    boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
    
    while ( ! radio.available() ){                             // While nothing is received
      if (micros() - started_waiting_at > 200000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
          timeout = true;
          break;
      }      
    }
        
    if ( timeout ){                                             // Describe the results
        Serial.println(F("Failed, response timed out."));
    }else{
        int success;                                 // Grab the response, compare, and send to debugging spew
        radio.read( &success, sizeof(success) );
        unsigned long end_time = micros();
        
        // Spew it
        Serial.print(F("Got response "));
        Serial.print(success);
        Serial.print(F(", Round-trip delay "));
        Serial.print(end_time-start_time);
        Serial.println(F(" microseconds"));
    }

    // Try again 1s later
    delay(1000);
  
} // Loop
