
/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"
#include <printf.h>
#include <ServoEasing.hpp>
//define servos
ServoEasing myXServo;
ServoEasing myYServo;
/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 0;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(53,8);
/**********************************************************/

byte addresses[][6] = {"1Node","2Node"};
int data[4];
//define indexes in data array to avoid confusion
int xAxisIndex = 0;
int yAxisIndex = 1;
int laserIndex = 2;
int winIndex = 3;
 
// Used to control whether this node is sending or receiving
bool role = 0;

/***************Servo config*******************/
int exe_interval = 250;
unsigned long lastLaserExecute = 0;

//configure pins
int xServoPin = 2;
int yServoPin = 3;
int laserPin = 4;
// configure value stores
int laserState = LOW;
int win =0;
int xValue = 0;
int prevXValue = 0;
int yValue = 0;
int prevYValue = 0;

void setup() {
  Serial.begin(115200);
  printf_begin();
  Serial.println(radio.begin());
  
  radio.setAutoAck(false);
  radio.setChannel(78);
  radio.setRetries(15,15);
    

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
  Serial.println(F("Reciever initializing"));
  radio.printDetails();

  //Configure servos and button
   pinMode(laserPin, OUTPUT);
  digitalWrite(laserPin, LOW);
  
  myXServo.attach(xServoPin );
  myXServo.easeTo(88,20);
  Serial.print("Writing initial positions");
  myYServo.attach(yServoPin);
  myYServo.easeTo(88,20);

  delay(1000);

  // Now move faster
  myXServo.setSpeed(100);  // This speed is taken if no speed argument is given.
  myYServo.setSpeed(100);
}

void loop() {
    readData();   
    executeCommands(); 
} // Loop

void executeCommands(){
  unsigned long currentMillis = millis();

  //move servo
  xValue = data[xAxisIndex];
  yValue = data[yAxisIndex];
  myYServo.startEaseTo(yValue);
  myXServo.startEaseTo(xValue);
  synchronizeAllServosAndStartInterrupt(false);
  
  do {
        // here you can call your own program
        delay(REFRESH_INTERVAL / 1000); // optional 20ms delay - REFRESH_INTERVAL is in Microseconds
    } while (!updateAllServos());

    if(data[laserIndex] != laserState && lastLaserExecute - currentMillis > exe_interval ){
      Serial.print(" changing laser state to : ");
      Serial.println(data[laserIndex]);
      digitalWrite(laserPin, data[laserIndex]);
      laserState = data[laserIndex];
    }
 
}

void readData(){
  if( radio.available()){
                                                                    // Variable for the received timestamp
      while (radio.available()) {                                   // While there is data ready
        radio.read( &data, sizeof(data) );             // Get the payload
      }

     int received = 1;
      radio.stopListening();                                        // First, stop listening so we can talk   
      radio.write( &received, sizeof(int) );              // Send the final one back.      
      radio.startListening();                                       // Now, resume listening so we catch the next packets.     
      Serial.print(F("Got Data : "));
      Serial.print("x cord :");  
      Serial.print(data[0]);
      Serial.print(", y cord :");  
      Serial.print(data[1]);
      Serial.print(", laser control :");  
      Serial.print(data[2]);
      Serial.print(", win scenario:");  
      Serial.println(data[3]);
   }
}
