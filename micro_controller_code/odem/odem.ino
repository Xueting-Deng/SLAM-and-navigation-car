#include <micro_ros_arduino.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <geometry_msgs/msg/twist.h>
#include <Motor.h>
#include <PidController.h>
#include <Kinematics.h>
#include <nav_msgs/msg/odometry.h>
#include <rosidl_runtime_c/string_functions.h>
//#include <micro_ros_utilities/string_utilities.h>
#include <rosidl_runtime_c/string_functions.h>

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_subscription_t subscriber;
geometry_msgs__msg__Twist sub_msg;
nav_msgs__msg__Odometry odom_msg;
rcl_publisher_t odom_publisher;
rcl_timer_t timer;

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


long Encoder1value = 0;
long Encoder2value = 0;

long Encoder1value_old = 0;
long Encoder2value_old = 0;

unsigned long last_time;
unsigned long dt;
unsigned long last_print_time;
unsigned long interval = 50;
float current_motor_speed[2];
float mm_pulse = 0.5140788; //mm
const float car_wheel_distance = 0.170; //m

float car_linear_speed = 0.0;
float car_angular_speed = 0.0;

// Wheel speed and pid controll
float kp_m = 100;
float ki_m = 5.0;
float kd_m = 0.0;
float out_pid_value[2];
PidController pid_controller[2];

//Motor set
Motor motor[2];

//Kinematics
Kinematics kinematics;


//Setting PWM properties
const int freq = 20000;
const int pwmChannel1 = 0;
const int pwmChannel2 = 0;
const int resolution = 10;
int dutyCycle[2];

//Odemetry value
float x = 0.0;
float y = 0.0;
float yaw = 0.0;

float quaternion_x = 0.0;
float quaternion_y = 0.0;
float quaternion_z = 0.0;
float quaternion_w = 0.0;

unsigned long previousMillis = 0;
 
void setup() {
  
  //sets encoder up
  pinMode(encoder1PinA, INPUT);
  pinMode(encoder1PinB, INPUT);
  pinMode(encoder2PinA, INPUT);
  pinMode(encoder2PinB, INPUT); 
  
  //motor set up
  motor[0].set_pin(motor1Pin1, motor1Pin2, enable1Pin);
  motor[1].set_pin(motor2Pin1, motor2Pin2, enable2Pin);
  motor[0].set_pwm(freq, resolution, pwmChannel1);
  motor[1].set_pwm(freq, resolution, pwmChannel2);

  motor[0].motor_on();
  motor[1].motor_on();

  attachInterrupt(digitalPinToInterrupt(encoder1PinA), updateEncoder1, RISING);
  attachInterrupt(digitalPinToInterrupt(encoder2PinA), updateEncoder2, RISING);
  Serial.begin(115200);

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
//   Serial.print("speed_1:");
//   Serial.print(current_motor_speed[0]);
//   Serial.print("\t");
//   Serial.print("speed_2:");
//   Serial.println(current_motor_speed[1]);
  // calculate the pid output
  out_pid_value[0] = pid_controller[0].update(current_motor_speed[0]);
  out_pid_value[1]= pid_controller[1].update(current_motor_speed[1]);
  //Serial.printf("The pid output is - 1: %f - 2: %f\n", out_pid_value[0], out_pid_value[1]);
  // update pmw parameters according to out_pid_value
  dutyCycle[0] = constrain(dutyCycle[0]+int(out_pid_value[0]),0,1023);
  dutyCycle[1] = constrain(dutyCycle[1]+int(out_pid_value[1]),0,1023);
  //Serial.printf("The dutycycle is - 1: %d - 2: %d\n", dutyCycle[0], dutyCycle[1]);
  motor[0].motor_dutycycle_update(dutyCycle[0]);
  motor[1].motor_dutycycle_update(dutyCycle[1]);
  delay(50);

}

void update_speed()
{
  unsigned long currentMillis = millis();
  
  dt = millis() - last_time;//millis returns ms
  current_motor_speed[0] = abs(float((Encoder1value - Encoder1value_old )* mm_pulse)/dt); // unit is m/s
  current_motor_speed[1] = abs(float((Encoder2value - Encoder2value_old) * mm_pulse)/dt);
  Encoder1value_old = Encoder1value;
  Encoder2value_old = Encoder2value; 
  
  kinematics.kinematics_forward(current_motor_speed[0], current_motor_speed[1], car_wheel_distance, car_linear_speed, car_angular_speed);
  kinematics.update_odom(car_linear_speed, car_angular_speed, dt, x, y, yaw);
  kinematics.convert_quaternion(0.0, 0.0, yaw, quaternion_x, quaternion_y, quaternion_z, quaternion_w);

  if (currentMillis - previousMillis >= 5000) {
  Serial.printf("t is: %f\n",dt);
  Serial.printf("motor1 speed: %f \t, motor2 speed: %f \n", current_motor_speed[0], current_motor_speed[1]);
  Serial.printf("linear speed: %f \t, angular speed: %f \n", car_linear_speed, car_angular_speed);
  Serial.printf("distance on x: %f \t, on y: %f \t, on yaw: %f \n", x, y, yaw);
  Serial.printf("quaternion x: %f \t, y: %f \t, z: %f \t, w: %f \n", quaternion_x, quaternion_y, quaternion_z, quaternion_w);
  previousMillis = currentMillis;
  }
  
  last_time = millis();  

}

void twist_callback(const void *msg_in)
{
  const geometry_msgs__msg__Twist *twist_msg = (const geometry_msgs__msg__Twist *)msg_in;
  float linear_x = twist_msg->linear.x;
  float angular_z = twist_msg->angular.z;
  
  float target_motor_speed_1, target_motor_speed_2;
  kinematics.kinematics_inverse(linear_x, angular_z*10, car_wheel_distance, target_motor_speed_1, target_motor_speed_2);
  //Serial.printf("target motor 1 - %f\t,target motor 2 - %f\n", target_motor_speed_1, target_motor_speed_2);
  
  if (target_motor_speed_1 == 0)
  {
    motor[0].motor_off();
  } else if (target_motor_speed_1 > 0){
    motor[0].motor_forward();
  } else {
    motor[0].motor_backward();
  }

  if (target_motor_speed_2 == 0)
  {
    motor[1].motor_off();
  } else if (target_motor_speed_2 > 0){
    motor[1].motor_forward();
  } else {
    motor[1].motor_backward();
  }
  pid_controller[0].update_target(target_motor_speed_1);
  pid_controller[1].update_target(target_motor_speed_2);
}

void timer_callback(rcl_timer_t* timer, int64_t last_call_time)
{
  // publish the odom_msg
    int64_t stamp = rmw_uros_epoch_millis();

    // input the velocity, position and other odometry message in ROS odom_msg
    odom_msg.header.stamp.sec = static_cast<int32_t>(stamp/1000);
    odom_msg.header.stamp.nanosec = static_cast<int32_t>((stamp%1000)*1e6);
    odom_msg.pose.pose.position.x = x;
    odom_msg.pose.pose.position.y = y;
    odom_msg.pose.pose.orientation.x = quaternion_x;
    odom_msg.pose.pose.orientation.y = quaternion_y;
    odom_msg.pose.pose.orientation.z = quaternion_z;
    odom_msg.pose.pose.orientation.w = quaternion_w;

    odom_msg.twist.twist.linear.x = car_linear_speed;
    odom_msg.twist.twist.angular.z = car_angular_speed;

    if(rcl_publish(&odom_publisher, &odom_msg, NULL) != RCL_RET_OK)
    {
      Serial.println("Publisher fialed to publish");
    }
  
}

void microros_task(void *param)
{

  // set up the wifi agent in micro-ros, wifi name - password - ip address
  set_microros_wifi_transports("MyAltice b69c5b", "crimson-404-750", "192.168.1.77", 8888);
  // wait for 2s for conneting the internet
  delay(2000);
  
  // 设置 micro-ROS 支持结构、节点和订阅。
  allocator = rcl_get_default_allocator();
  rclc_support_init(&support, 0, NULL, &allocator);
  // set the node
  rclc_node_init_default(&node, "car", "", &support);

  unsigned int num_handles = 2;
  // 设置 micro-ROS 执行器，并将订阅添加到其中。
  rclc_executor_init(&executor, &support.context, num_handles, &allocator);
  // initialize a subcriber
  rclc_subscription_init_default(&subscriber, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist), "/cmd_vel");
  rclc_executor_add_subscription(&executor, &subscriber, &sub_msg, &twist_callback, ON_NEW_DATA);
  // initialize odom_msg
//  odom_msg.header.frame_id = micro_ros_string_utilities_set(odom_msg.header.frame_id, "odom");
//  odom_msg.child_frame_id = micro_ros_string_utilities_set(odom_msg.child_frame_id, "base_link");
  rosidl_runtime_c__String__init(&odom_msg.header.frame_id);
  rosidl_runtime_c__String__init(&odom_msg.child_frame_id);
  rosidl_runtime_c__String__assign(&odom_msg.header.frame_id, "odom");
  rosidl_runtime_c__String__assign(&odom_msg.child_frame_id, "base_link");
  //initialize the publisher and timer
  rclc_publisher_init_default(&odom_publisher, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry), "/odom"); 
  //rcl_ret_t ret = rclc_publisher_init_best_effort(&odom_publisher, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry), "/odom");
  
  rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(50), timer_callback);
//    if (ret != RCL_RET_OK) {
//    Serial.println("Timer init failed");
//  }
  rclc_executor_add_timer(&executor, &timer);

  //sync the time of the micro-controller and system
//  if(rmw_uros_sync_session(1000) == RCL_RET_OK) // verified, successed
//    {
//      Serial.println("Synchronize success 1");
//    }
  
  while(!rmw_uros_epoch_synchronized())
  {
      Serial.println("Hasn't got synchronized"); 
      rmw_uros_sync_session(1000); // if synchronized, set the time as micro-ros agent time, and output the message
      delay(10); 
      if(rmw_uros_sync_session(1000) == RCL_RET_OK)
        {
          Serial.println("Synchronize success 2");
        }
  }
  // 循环运行 micro-ROS 执行器以处理传入的消息。
  // spin the executor
  rclc_executor_spin(&executor);

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
