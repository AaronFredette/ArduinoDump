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

int servoMovementInterval = 50;           // The higher the number, the slower the timing.
int joyStickReadInterval = 100;

int exe_interval = 250;
int xValue = 0;
int prevXValue = 0;
int yValue = 0;
int prevYValue = 0;
int bValue = 0;
int btnPin = 8;

int xServoPin = 9;
int yServoPin = 10;
int laserPin = 7;
int laserState = LOW;

unsigned long lastServoExecuteMillis = 0;
unsigned long lastExecutedMillis = 0;
unsigned long lastJoyStickReadMillis = 0;

void setup() {
  //wire up joystick button
  pinMode(btnPin, INPUT);
  digitalWrite(btnPin, HIGH);

  pinMode(laserPin, OUTPUT);
  digitalWrite(laserPin, LOW);

  myXServo.attach(xServoPin );
  myXServo.write(0);

  myYServo.attach(yServoPin);
  myYServo.write(0);
  delay(15);
  Serial.begin(9600);
  dht.begin();
}

void loop() {



  unsigned long currentMillis = millis();

  // slow down joy stick input by only polling the input every 10th of second
  if (currentMillis - lastJoyStickReadMillis >= joyStickReadInterval) {
    //joy stick
    //map(value, fromLow, fromHigh, toLow, toHigh)
    xValue = (map(analogRead(A0), 0, 1023, 5, 175) * .5) + (prevXValue * .5);
    yValue = (map(analogRead(A1), 0, 1023, 5, 175) * .5) + (prevYValue * .5);


    lastJoyStickReadMillis = currentMillis;
    Serial.print("joystick read");
  }

  bValue = digitalRead(btnPin);

  if (currentMillis - lastServoExecuteMillis >= servoMovementInterval) {

    int targetXValue = 0;
    int xDiff = xValue - prevXValue;
    /*Serial.print("xdiff:");
      Serial.print(xDiff);
      Serial.print(", xvalue:");
      Serial.print(xValue,DEC);
      Serial.print(", prevXvalue:");
      Serial.print(prevXValue,DEC);
      Serial.print(", abs(xDiff):");
      Serial.print(abs(xDiff));
      Serial.print("\n");*/

    if (prevXValue != xValue && abs(xDiff) > 1) {
      if (prevXValue < xValue) {
        targetXValue = xValue + 1;
      } else {
        targetXValue = xValue - 1;
      }

      myXServo.write(xValue);
      Serial.print(targetXValue);
      Serial.print(",");
      prevXValue = xValue;
    }

    int targetYValue = 0;
    int yDiff = yValue - prevYValue;
    if (prevYValue != yValue && abs(yDiff) > 1) {
      if (prevYValue < yValue) {
        targetYValue = yValue + 1;
      } else {
        targetYValue = yValue - 1;
      }


      myYServo.write(targetYValue);
      Serial.print(targetYValue);
      prevYValue = targetYValue;
    }

    lastServoExecuteMillis = currentMillis;
  }

  if (currentMillis - lastExecutedMillis >= exe_interval) {
    if (bValue == LOW) {
      if (laserState == LOW) {
        laserState = HIGH;
      } else {
        laserState = LOW;
      }
      digitalWrite(laserPin, laserState);
      lastExecutedMillis = currentMillis; // save the last executed time
    }
  }

}
