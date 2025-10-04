
#include <PidController.h>
//
//rclc_executor_t executor;
//rclc_support_t support;
//rcl_allocator_t allocator;
//rcl_node_t node;
//rcl_subscription_t subscriber;
//geometry_msgs__msg__Twist sub_msg;

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

volatile long Encoder1value_old = 0;
volatile long Encoder2value_old = 0;

float last_time;
float dt;
float current_motor_speed[2];
float mm_pulse = 0.5140788; //mm

// Wheel speed and pid controll
float kp_m_0 = 100.0;
float ki_m_0 = 5.0;
float kd_m_0 = 0.0;
float kp_m_1 = 100.0;
float ki_m_1 = 5.0;
float kd_m_1 = 0.0;
float out_pid_value[2];
PidController pid_controller[2];

//Setting PWM properties
const int freq = 20000;
const int pwmChannel1 = 0;
const int pwmChannel2 = 1;
const int resolution = 10;
int dutyCycle[2];
 
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

  attachInterrupt(digitalPinToInterrupt(encoder1PinA), updateEncoder1, RISING);
  attachInterrupt(digitalPinToInterrupt(encoder2PinA), updateEncoder2, RISING);
  Serial.begin(115200);

  ledcAttachChannel(enable1Pin, freq, resolution, pwmChannel1);
  ledcAttachChannel(enable2Pin, freq, resolution, pwmChannel2);

  //pid set up
  pid_controller[0].update_pid(kp_m_0, ki_m_0, kd_m_0);
  pid_controller[1].update_pid(kp_m_1, ki_m_1, kd_m_1);
  pid_controller[0].out_limit(-100, 100);
  pid_controller[1].out_limit(-100, 100); 

  dutyCycle[0] = 500;
  dutyCycle[1] = 500;
}

void loop() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);  
  float target = 0.2;
  pid_controller[0].update_target(target);
  pid_controller[1].update_target(target);
  Serial.print("target:");
  Serial.print(target*100);
  Serial.print("\t");
  
  update_speed();
  Serial.print("speed_1:");
  Serial.print(current_motor_speed[0]*100);
  Serial.print("\t");
  Serial.print("speed_2:");
  Serial.println(current_motor_speed[1]*100);
  //Serial.printf("The encoder of motor is - motor 1: %ld - motor 2: %ld\n", Encoder1value, Encoder2value);
  //Serial.printf("\nThe speed of motor is - motor 1: %f - motor 2: %f\n", current_motor_speed[0], current_motor_speed[1]);
  
  // calculate the pid output
  out_pid_value[0] = pid_controller[0].update(current_motor_speed[0]);
  out_pid_value[1]= pid_controller[1].update(current_motor_speed[1]);
  //Serial.printf("The pid output is - 1: %f - 2: %f\n", out_pid_value[0], out_pid_value[1]);
  // update pmw parameters according to out_pid_value
  dutyCycle[0] = constrain(dutyCycle[0]+int(out_pid_value[0]),0,1023);
  dutyCycle[1] = constrain(dutyCycle[1]+int(out_pid_value[1]),0,1023);
  //Serial.printf("The dutycycle is - 1: %d - 2: %d\n", dutyCycle[0], dutyCycle[1]);
  ledcWrite(enable1Pin, dutyCycle[0]);
  ledcWrite(enable2Pin, dutyCycle[1]);
  delay(100);

}

void update_speed()
{
  dt = millis() - last_time;//millis returns s^-3
  current_motor_speed[0] = abs(float((Encoder1value - Encoder1value_old )* mm_pulse)/dt); // unit is m/s
  //Serial.println(Encoder1value);
  current_motor_speed[1] = abs(float((Encoder2value - Encoder2value_old) * mm_pulse)/dt);
  //Serial.println(Encoder2value);
  Encoder1value_old = Encoder1value;
  Encoder2value_old = Encoder2value; 
  last_time = millis();  

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
