#include <Servo.h>
int IRSensor = 9;
Servo myservo;

void setup(){
  myservo.attach(3);
  Serial.begin(115200);
  Serial.println("Serial Working");
  pinMode(IRSensor, INPUT); 
}

void loop(){
  int sensorStatus = digitalRead(IRSensor); 
  if (sensorStatus == 1) 
  {
    myservo.write(75);
    Serial.println("Motion Detected!"); 
  }
  else  {
    myservo.write(0);             
    Serial.println("Motion Ended!");
  }
}