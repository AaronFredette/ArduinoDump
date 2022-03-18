
/*
  Getting Started example sketch for nRF24L01+ radios
  This is a very basic example of how to send data from one node to another
  Updated: Dec 2014 by TMRh20
*/

// RF24 lib set up
#include <SPI.h>
#include "RF24.h"
#include <printf.h>


/****************** Radio Setup***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 1;
bool role = 1; // Used to control whether this node is sending or receiving // 1= transmit,  0 = receive

RF24 radio(10, 8); // ce, cn
//byte addresses[][6] = {"1Node", "2Node"};
const byte address[6] = "06720";
/**********************************************************/

/***************** Joy Stick set up ********************************/

int exe_interval = 250; // used to determine how frequently joy stick is polled

//define indexes in array to avoid confusion
int xAxisIndex = 0;
int yAxisIndex = 1;
int laserIndex = 2;
int winIndex = 3;
// control send frequency
unsigned long lastSendmillis = 0;

//initial set of values while waiting
int controllerData[4] = {20, 20, 1, 0}; // x cord, y cord, laser instruct, win scenario

// used for slowing down the reads of the joy stick
int joyStickReadInterval = 100;
unsigned long lastJoyStickReadMillis = 0;
unsigned long lastExecutedMillis = 0;

//set initial values
int laserState = LOW; // keep track of laser state to determine if we should update
int winState = LOW;
int bValue = 0;
int xValue = 0;
int yValue = 0;

//used to keep track of previous read values to 'smooth' out control
int prevYValue = 0;
int prevXValue = 0;

//set pins
int btnPin = 5;
int jsXPin = A0;
int jsYPin = A1;
int winPin = 4;


/**********************************************************/



void setup() {
  Serial.begin(115200);
  printf_begin();

  Serial.println(radio.begin());
  radio.setAutoAck(false);
  radio.setRetries(15, 15);
  radio.setChannel(85);
  Serial.println(F("Starting joystick transmitter"));


  // Set the PA Level low to prevent power supply related issues since this is a
  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);

  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openWritingPipe(address);

  radio.stopListening(); // Start the radio listening for data
  radio.printDetails();


  //wire up joystick button
  pinMode(btnPin, INPUT);
  pinMode(winPin, INPUT);
  digitalWrite(btnPin, HIGH);
}

void loop() {

  readJoyStick();
  transmit();
} // Loop

void readJoyStick() {
  unsigned long currentMillis = millis();

  // slow down joy stick input by only polling the input every 10th of second
  if (currentMillis - lastJoyStickReadMillis >= joyStickReadInterval) {

    xValue = analogRead(A0);
    yValue = analogRead(A1);
    //add values to data array
    controllerData[xAxisIndex] = xValue;
    controllerData[yAxisIndex] = yValue;
    controllerData[laserIndex] = !digitalRead(btnPin);
    controllerData[winIndex] = !digitalRead(winPin);
    
    lastJoyStickReadMillis = currentMillis;
  }
}

void transmit() {
  unsigned long currentMillis = millis();

  // execute a simlar rate as joystick
  if (currentMillis - lastSendmillis >= joyStickReadInterval) {
    
    /*Serial.println(F("Now sending: "));
    Serial.print("x cord : ");
    Serial.println(controllerData[xAxisIndex]);
    Serial.print("y cord : ");
    Serial.println(controllerData[yAxisIndex]);
    Serial.print("laser command : ");
    Serial.println(controllerData[laserIndex]);
    Serial.print("win scenario : ");
    Serial.println(controllerData[winIndex]);*/

    radio.write(&controllerData, sizeof(controllerData));
    lastSendmillis = millis();
  }

}
