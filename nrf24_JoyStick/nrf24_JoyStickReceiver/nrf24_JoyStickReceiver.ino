
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
ServoEasing treatServo;
/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 0;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(10,8); // pro mini ce, csn
//RF24 radio(53,8); // mega ce, csn
/**********************************************************/

//byte addresses[][6] = {"1Node","2Node"};
const byte address[6] = "06720";
int data[4];
//define indexes in data array to avoid confusion
int xAxisIndex = 0;
int yAxisIndex = 1;
int laserIndex = 2;
int winIndex = 3;
 
// Used to control whether this node is sending or receiving
bool role = 0;

/***************Servo config*******************/
int exe_interval = 400;
unsigned long lastLaserExecute = 0;

//configure pins
int xServoPin = 2;
int yServoPin = 3;
int laserPin = 4;
int treatServoPin = 5;

// configure value stores
int laserState = LOW;
int win = 0;
int xValue = 0;
int prevXValue = 0;
int yValue = 0;
int prevYValue = 0;
bool treatDispensed = false;

//treat servo positions
int treatStandBy = 120; // closed will also line up the dispensing holes
int treatReload = 30;

//configure auto turn off variables
int offPin = 7;
unsigned long lastUpdateMillis = 0;
int turnOffAfterEllapsed = 30000; // 3minutes of no action

void setup() {
  Serial.begin(115200);
  printf_begin();
  Serial.println(radio.begin());
  
  radio.setAutoAck(false);
  radio.setChannel(85);
  radio.setRetries(15,15);
    

  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_MIN);
  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openReadingPipe(0,address);
  radio.setDataRate(RF24_250KBPS);
    
  // Start the radio listening for data
  radio.startListening();
  Serial.println(F("Reciever initializing"));
  radio.printDetails();

  //Configure servos and button
  pinMode(laserPin, OUTPUT);
  digitalWrite(laserPin, LOW);

  pinMode(offPin, OUTPUT);
  digitalWrite(offPin, LOW);
  
  myXServo.attach(xServoPin );
  myXServo.easeTo(88,20);
  
  myYServo.attach(yServoPin);
  myYServo.easeTo(70,20);

  treatServo.attach(treatServoPin);
  treatServo.easeTo(treatStandBy,20);

  delay(1000);

  // Now move faster
  myXServo.setSpeed(150);  // This speed is taken if no speed argument is given.
  myYServo.setSpeed(150);
  treatServo.setSpeed(100);

  lastUpdateMillis = millis();
}

void loop() {

//To DO : treat dispense code is never being called.
    readData();
   // if(!win){
      executeCommands(); 
   // }else if (!treatDispensed){
  //    dispenseTreat();
  //  }

    if(millis() - lastUpdateMillis > turnOffAfterEllapsed){
      Serial.println("turning off");
      digitalWrite(offPin, HIGH);
    }
} // Loop

void dispenseTreat(){
  //move laser to point at base of slide
  myYServo.startEaseTo(40);
  myXServo.startEaseTo(70);
  synchronizeAllServosAndStartInterrupt(false);
  treatServo.easeTo(treatReload);
  do {
        delay(REFRESH_INTERVAL / 1000); // optional 20ms delay - REFRESH_INTERVAL is in Microseconds
    } while (!updateAllServos());
    
  Serial.println("dispense");
  delay(1000);
  
  treatServo.easeTo(treatStandBy);
  delay(1000);
  treatDispensed = true;
  
}

void executeCommands(){
  unsigned long currentMillis = millis();
 
   xValue = (map(data[xAxisIndex], 0, 1023, 25, 155));
   yValue = (map(data[yAxisIndex], 0, 1023, 25, 155));
 
  myYServo.startEaseTo(yValue);
  myXServo.startEaseTo(xValue);
  synchronizeAllServosAndStartInterrupt(false);

    /*Serial.print(F("Writing Data : "));
      Serial.print("x cord :");  
      Serial.print(xValue);
      Serial.print(", y cord :");  
      Serial.print(yValue);
      Serial.print(", laser control :");  
      Serial.print(data[2]);
      Serial.print(", win scenario:");  
      Serial.println(data[3]);*/
  
  do {
        // here you can call your own program
        delay(REFRESH_INTERVAL / 500); // optional 20ms delay - REFRESH_INTERVAL is in Microseconds
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

      win = data[winIndex];
      // reset the game if treat was dispensed but the game is no longer a win
      if(treatDispensed && !win){
        treatDispensed = false;
      }
      
      /*Serial.print(F("Got Data : "));
      Serial.print("x cord :");  
      Serial.print(data[0]);
      Serial.print(", y cord :");  
      Serial.print(data[1]);
      Serial.print(", laser control :");  
      Serial.print(data[2]);
      Serial.print(", win scenario:");  
      Serial.println(data[3]);*/


      //TO DO : Check if data is different by more than 5 degrees and update the 
      // 'last updated' time turn off if greater than 3 mins 
   }
}
