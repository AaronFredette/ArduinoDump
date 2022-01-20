#include <DHT.h>
#include <DHT_U.h>
#include <Servo.h>

#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
Servo myXServo;
Servo myYServo;
/*
  For Loop Iteration

  Demonstrates the use of a for() loop.
  Lights multiple LEDs in sequence, then in reverse.

  The circuit:
  - LEDs from pins 2 through 7 to ground

  created 2006
  by David A. Mellis
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/ForLoopIteration
*/

int timer = 15;           // The higher the number, the slower the timing.

int exe_interval = 250;
int xValue = 0;
int prevXValue = 0;
int yValue = 0;
int prevYValue = 0;
int bValue = 0;
int btnPin = 8;

int xServoPin = 9;
int yServoPin = 10;
int laserPin = 4;
int laserState = LOW;

unsigned long lastExecutedMillis = 0;

void setup() {
  //wire up joystick button
  pinMode(btnPin,INPUT);
  digitalWrite(btnPin,HIGH);
  
  pinMode(laserPin,OUTPUT);
  digitalWrite(laserPin,LOW);
  
  myXServo.attach(xServoPin );
  myXServo.write(0);
  
  myYServo.attach(yServoPin);
  myYServo.write(0);
  delay(15);
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  
    //joy stick
    //map(value, fromLow, fromHigh, toLow, toHigh)
    xValue = (map(analogRead(A0),0,1023,0,180) * .15) + (prevXValue * .85);
    yValue = (map(analogRead(A1),0,1023,0,180) * .15) + (prevYValue * .85);
    bValue = digitalRead(btnPin);

    
    myXServo.write(xValue);
    myYServo.write(yValue);
    unsigned long currentMillis = millis();
    if (currentMillis - lastExecutedMillis >= exe_interval) {
      if(bValue == LOW){
        if(laserState == LOW){
          laserState = HIGH;
          Serial.print("turning laser on \n");
        }else{
          laserState = LOW;
          Serial.print("turning laser off \n");
        }
        digitalWrite(laserPin,laserState);
        lastExecutedMillis = currentMillis; // save the last executed time
      }
    }
  
    
    
    /*Serial.print(xValue,DEC);
    Serial.print(",");
    Serial.print(yValue,DEC);
    Serial.print(",");
    Serial.print(bValue);
    Serial.print("\n");*/
    delay(timer);
    prevXValue = xValue;
    prevYValue = yValue;
}
