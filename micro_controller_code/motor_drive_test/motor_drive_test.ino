// Set pin
// Motor 1
int motor1Pin1 = 7; 
int motor1Pin2 = 6; 
int enable1Pin = 3; 
int encoder1PinA = 12;
int encoder1PinB = 11;
// Motor 2
int motor2Pin1 = 5; 
int motor2Pin2 = 4; 
int enable2Pin = 10; 
int encoder2PinA = 13;
int encoder2PinB = 14;

////Setting PWM properties
const int freq = 3000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 200;

void setup() {
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enable2Pin, OUTPUT);

//  analogWriteResolution(enable1Pin, resolution);
//  analogWriteFrequency(enable1Pin, freq);
//  //analogWrite(enable1Pin,dutyCycle);
//  analogWriteResolution(enable2Pin, resolution);
//  analogWriteFrequency(enable2Pin, freq);
//  //analogWrite(enable2Pin,dutyCycle);

  ledcAttachChannel(enable1Pin, freq, resolution, pwmChannel);
  ledcAttachChannel(enable2Pin, freq, resolution, pwmChannel);


  Serial.begin(115200);
  Serial.print("Testing DC Motor...");
}

void loop() {
  digitalWrite(enable1Pin, HIGH);
  digitalWrite(enable2Pin, HIGH);
  //move forward with maximum speed
//  Serial.println("Moving Forward");
//  digitalWrite(motor1Pin1, LOW);
//  digitalWrite(motor1Pin2, HIGH);
//  digitalWrite(motor2Pin1, LOW);
//  digitalWrite(motor2Pin2, HIGH);
//  delay(1000);
//
//  // Stop motor 
//  Serial.println("Stop");
//  digitalWrite(motor1Pin1, LOW);
//  digitalWrite(motor1Pin2, LOW);
//  digitalWrite(motor2Pin1, LOW);
//  digitalWrite(motor2Pin2, LOW);
//  delay(1000);
//
//  // move backward 
//  Serial.println("Moving Backward");
//  digitalWrite(motor1Pin1, HIGH);
//  digitalWrite(motor1Pin2, LOW);
//  digitalWrite(motor2Pin1, HIGH);
//  digitalWrite(motor2Pin2, LOW);
//  delay(1000);
//
//  
//  // Stop motor 
//  Serial.println("Stop");
//  digitalWrite(motor1Pin1, LOW);
//  digitalWrite(motor1Pin2, LOW);
//  digitalWrite(motor2Pin1, LOW);
//  digitalWrite(motor2Pin2, LOW);
//  delay(1000);
//
//  // turn right
//  Serial.println("Turning right");
//  digitalWrite(motor1Pin1, LOW);
//  digitalWrite(motor1Pin2, HIGH);
//  digitalWrite(motor2Pin1, HIGH);
//  digitalWrite(motor2Pin2, LOW);
//  delay(1000);
//
//  
//  // Stop motor 
//  Serial.println("Stop");
//  digitalWrite(motor1Pin1, LOW);
//  digitalWrite(motor1Pin2, LOW);
//  digitalWrite(motor2Pin1, LOW);
//  digitalWrite(motor2Pin2, LOW);
//  delay(1000);
//
//  
//  Serial.println("Turning left");
//  digitalWrite(motor1Pin1, HIGH);
//  digitalWrite(motor1Pin2, LOW);
//  digitalWrite(motor2Pin1, LOW);
//  digitalWrite(motor2Pin2, HIGH);
//  delay(1000);

  // move motor forward with increasing speed
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
  while (dutyCycle <= 255){
    ledcWrite(enable1Pin, dutyCycle);
    ledcWrite(enable2Pin, dutyCycle);
    Serial.println("Forward with duty cycle:");
    Serial.println(dutyCycle);
    dutyCycle = dutyCycle +5;
    delay(500);
    }  
    dutyCycle = 200;
  
}
