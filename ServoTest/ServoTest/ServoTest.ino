#include <DHT.h>
#include <DHT_U.h>
#include <Servo.h>

#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
Servo myXServo;
Servo myYServo;

int servoAngle = 0;
unsigned long lastServoExecutedMillis = 0;
int servoInterval = 50;
int xServoPin = 9;
int yServoPin = 10;
void setup() {
  // put your setup code here, to run once:
  myXServo.attach(xServoPin );
  myXServo.write(0);

  myYServo.attach(yServoPin);
  myYServo.write(0);
  delay(15);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  if (currentMillis - lastServoExecutedMillis > servoInterval) {
    Serial.print("moving servo \n");
    Serial.print(servoAngle);
    if (servoAngle >= 179) {
      servoAngle-= 1;
    } else {
      servoAngle+= 1;
    }
    lastServoExecutedMillis = currentMillis;
    myXServo.write(servoAngle);
  }
  
}
