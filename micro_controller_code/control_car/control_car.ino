#include <micro_ros_arduino.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <geometry_msgs/msg/twist.h>

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_subscription_t subscriber;
geometry_msgs__msg__Twist sub_msg;

// Set pin
// #define LED_PIN 2
int motor1Pin1 = 4;
int motor1Pin2 = 5;
int enable1Pin = 3;

int motor2Pin1 = 6;
int motor2Pin2 = 7;
int enable2Pin = 10;

////Setting PWM properties
//const int freq = 3000;
//const int pwmChannel = 0;
//const int resolution = 8;
//int dutyCycle = 200;

void twist_callback(const void *msg_in)
{
  const geometry_msgs__msg__Twist *twist_msg = (const geometry_msgs__msg__Twist *)msg_in;
  float linear_x = twist_msg->linear.x;
  float angular_z = twist_msg->angular.z;
  // Stop motor
  if (linear_x == 0 && angular_z ==0)
  {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);   
  }
  // move forward
  if (linear_x > 0 )
  {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);   
  }

  // move backward
  if (linear_x < 0)
  {
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);   
  }

  // turn right
  if (angular_z > 0 )
  {
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);   
  }
  // turn left
  if (angular_z < 0 )
  {
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);   
  }

}


void setup() {
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enable2Pin, OUTPUT);

  // analogWriteResolution(enable1Pin, resolution);
  // analogWriteFrequency(enable1Pin, freq);
  // analogWrite(enable1Pin,dutyCycle);


  // ledcAttachChannel(enable1Pin, freq, resolution, pwmChannel);
  // ledcAttachChannel(enable2Pin, freq, resolution, pwmChannel);

  digitalWrite(enable1Pin, HIGH);
  digitalWrite(enable2Pin, HIGH);

  //Serial.begin(1152000);
  //set_microros_transports();
  set_microros_wifi_transports("MyAltice b69c5b", "crimson-404-750", "192.168.1.77", 8888);
  delay(2000);

  allocator = rcl_get_default_allocator();
  rclc_support_init(&support, 0, NULL, &allocator);

  // node
  rclc_node_init_default(&node, "control_car_node", "", &support);
  rclc_executor_init(&executor, &support.context, 1, &allocator);

  //Subscriber
  rclc_subscription_init_default(&subscriber,&node, ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),"/cmd_vel");
  rclc_executor_add_subscription(&executor, &subscriber, &sub_msg, &twist_callback, ON_NEW_DATA);
  //pinMode(2, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
}
