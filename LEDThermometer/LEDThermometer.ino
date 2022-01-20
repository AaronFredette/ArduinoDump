#include <DHT.h>
#include <DHT_U.h>
#include <Servo.h>

#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
Servo myServo;
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
int cold = 3;
int warm = 2;
int justRight = 4;
int currentTempPin;

int xValue = 0;
int yValue = 0;
int bValue = 0;
int btnPin = 8;

int servoPin = 9;

void setup() {
  // use a for loop to initialize each pin as an output:
  for (int thisPin = 2; thisPin < 5; thisPin++) {
    pinMode(thisPin, OUTPUT);
  }

  pinMode(btnPin,INPUT);
  digitalWrite(btnPin,HIGH);
  myServo.attach(9);
  myServo.write(0);
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  
  float f = dht.readTemperature(true);
  //Serial.print(F("Temperature: "));
  //Serial.println(f);
  //TurnOnPin
  digitalWrite(currentTempPin, LOW);
  if(f >= 65){
    currentTempPin = warm;
  }else if(f < 65 && f > 58){
    currentTempPin = justRight;
   }else {
    currentTempPin = cold;
   }
  
    digitalWrite(currentTempPin, HIGH);
  
    //joy stick
    xValue = analogRead(A0)/6.7;
    yValue = analogRead(A1)/5.7;
    bValue = digitalRead(btnPin);

    
    
      myServo.write(yValue);
      //delay(15);
    
    Serial.print(xValue,DEC);
    Serial.print(",");
    Serial.print(yValue,DEC);
    Serial.print(",");
    Serial.print(!bValue);
    Serial.print("\n");
    delay(timer);
}
