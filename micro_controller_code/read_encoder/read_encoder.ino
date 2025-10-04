#include <Arduino.h>

// Motor 1
int motor1Pin1 = 6; 
int motor1Pin2 = 7; 
int enable1Pin = 10; 
int encoder1PinA = 13;
int encoder1PinB = 14;

// Motor 2
int motor2Pin1 = 4; 
int motor2Pin2 = 5; 
int enable2Pin = 3; 
int encoder2PinA = 11;
int encoder2PinB = 12;


volatile long Encoder1value = 0;
volatile long Encoder2value = 0;


void setup() {
  //sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enable2Pin, OUTPUT);

  pinMode(encoder1PinA, INPUT);
  pinMode(encoder1PinB, INPUT);
  pinMode(encoder2PinA, INPUT);
  pinMode(encoder2PinB, INPUT);  
  
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(encoder1PinA), updateEncoder1, RISING);
  attachInterrupt(digitalPinToInterrupt(encoder2PinA), updateEncoder2, RISING);

}

void loop() {


  Serial.printf("The encoder of motor is - motor 1: %ld - motor 2: %ld\n", Encoder1value, Encoder2value);


}



void updateEncoder1()
{
  if (digitalRead(encoder1PinA) > digitalRead(encoder1PinB))
  Encoder1value = Encoder1value - 1;
  else
  Encoder1value = Encoder1value + 1;
}

void updateEncoder2()
{
  if (digitalRead(encoder2PinA) > digitalRead(encoder2PinB))
  Encoder2value = Encoder2value - 1;
  else
  Encoder2value = Encoder2value + 1;
}
