#include <micro_ros_arduino.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <geometry_msgs/msg/twist.h>
#include <PidController.h>

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_subscription_t subscriber;
geometry_msgs__msg__Twist sub_msg;

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
float kp_m = 100;
float ki_m = 5.0;
float kd_m = 0.0;
float out_pid_value[2];
PidController pid_controller[2];

//Setting PWM properties
const int freq = 20000;
const int pwmChannel1 = 0;
const int pwmChannel2 = 0;
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
  pid_controller[0].update_pid(kp_m, ki_m, kd_m);
  pid_controller[1].update_pid(kp_m, ki_m, kd_m);
  pid_controller[0].out_limit(-100, 100);
  pid_controller[1].out_limit(-100, 100); 

  dutyCycle[0] = 500;
  dutyCycle[1] = 500;
  
  // micros running in other core
  xTaskCreatePinnedToCore(microros_task, "microros_task", 10240, NULL, 1, NULL, 0);
}

void loop() {
  
  update_speed();
//  Serial.print("speed_1:");
//  Serial.print(current_motor_speed[0]);
//  Serial.print("\t");
//  Serial.print("speed_2:");
//  Serial.println(current_motor_speed[1]);
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
  delay(10);

}

void update_speed()
{
  Serial.printf("The encoder values are - 1: %d - 2: %d\n", Encoder1value, Encoder2value);
  dt = millis() - last_time;//millis returns s^-3
  current_motor_speed[0] = abs(float((Encoder1value - Encoder1value_old )* mm_pulse)/dt); // unit is m/s
  //Serial.println(Encoder1value);
  current_motor_speed[1] = abs(float((Encoder2value - Encoder2value_old) * mm_pulse)/dt);
  //Serial.println(Encoder2value);
  Encoder1value_old = Encoder1value;
  Encoder2value_old = Encoder2value; 
  last_time = millis();  

}

void twist_callback(const void *msg_in)
{
  const geometry_msgs__msg__Twist *twist_msg = (const geometry_msgs__msg__Twist *)msg_in;
  float linear_x = twist_msg->linear.x;
  float angular_z = twist_msg->angular.z;
  //Serial.printf("The target speeds are %f or %f\n",linear_x*2/33, angular_z/33);
  // Stop motor
  if (linear_x == 0 && angular_z ==0)
  {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);   
    Serial.print("target:");
    Serial.println(linear_x);
    pid_controller[0].update_target(0);
    pid_controller[1].update_target(0);
  }
  // move forward
  if (linear_x > 0 && angular_z == 0)
  {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);   
    Serial.print("target:");
    Serial.println(linear_x);
    pid_controller[0].update_target(linear_x);
    pid_controller[1].update_target(linear_x);
  }

  // move backward
  if (linear_x < 0 && angular_z == 0)
  {
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);
    Serial.print("target:");
    Serial.println(-linear_x);
    pid_controller[0].update_target(-linear_x);
    pid_controller[1].update_target(-linear_x);   
  }

  // turn right
  if (angular_z > 0 && linear_x == 0 )
  {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);  
    Serial.print("target:");
    Serial.println(angular_z/2);
    pid_controller[0].update_target(angular_z/2);
    pid_controller[1].update_target(angular_z/2);
  }
  // turn left
  if (angular_z < 0 && linear_x == 0 )
  {
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH); 
    Serial.print("target:");
    Serial.println(-angular_z/2);  
    pid_controller[0].update_target(-angular_z/2);
    pid_controller[1].update_target(-angular_z/2);
  }

}

void microros_task(void *param)
{

  // 使用 WiFi 网络和代理 IP 设置 micro-ROS 传输层。
  set_microros_wifi_transports("MyAltice b69c5b", "crimson-404-750", "192.168.1.77", 8888);

  // 等待 2 秒，以便网络连接得到建立。
  delay(2000);

  // 设置 micro-ROS 支持结构、节点和订阅。
  allocator = rcl_get_default_allocator();
  rclc_support_init(&support, 0, NULL, &allocator);
  rclc_node_init_default(&node, "pid_control_node", "", &support);
  rclc_subscription_init_default(
      &subscriber,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
      "/cmd_vel");

  // 设置 micro-ROS 执行器，并将订阅添加到其中。
  rclc_executor_init(&executor, &support.context, 1, &allocator);
  rclc_executor_add_subscription(&executor, &subscriber, &sub_msg, &twist_callback, ON_NEW_DATA);

  // 循环运行 micro-ROS 执行器以处理传入的消息。
  while (true)
  {
    delay(100);
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
  }
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
