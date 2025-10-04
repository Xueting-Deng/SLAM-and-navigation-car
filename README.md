This package primarily involves communication between the ESP32-S3 and the host machine via micro-ROS, including wifi settings, controlling the car by keyboard, PID control of the motor, and reading the odometer.

You don't need to download the whole package; just downloading the codes in the micro_controller_code file will be enough.

**Some Basic concepts of ROS2 reference:** https://articulatedrobotics.xyz/tutorials/ready-for-ros/ros-overview
## 1. Software Setup
1. Install ros2 (foxy) on host machine
2. Install Arduino IDE on the host machine, and install ESP32 board. Following this tutorial: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/. Except choose ESPS3 Dev Module board instead in the end. The port number will be ttyxxx in the ubuntu. (Connect ESP32 at USB-UART port) - choose 3.0.5 package in arduino ide
3. Add micro-ros arduino library to Arduino IDE. Download : https://github.com/micro-ROS/micro_ros_arduino (choose foxy or the version of ros2 you want to use). Go to Arduino IDE, click Sketch - include library - Add ZIP library, choose the downloaded zip file, click OK. To verify this, go to File - Example - micro_ros_arduino - micro-ros_publisher. Try to compile and upload it to ESP32. (I got an error here saying don't have pre-compile file names ESP32S3. I think this is because we are using ESP32-S3 board instead of a standard ESP32, so I went to the library location, go into src file, and changed the file named ESP32 to ESP32S3. I don't know if it's correct thing to do, but it works for now.)
4. Connect ESP32 with ROS2 - clone micro-ros source and install micro-ros agent. Following this tutorial, but choose the version of ros2 you want: https://www.hackster.io/514301/micro-ros-on-esp32-using-arduino-ide-1360ca. (this tutorial also has add micro-ros arduino library part, but I didn't follow it.)
5. Other reference: https://www.youtube.com/watch?v=qtVFsgTG3AA (Don't follow him for step4)
6. Wifi connection setup: go to arduino-ide, go to File - Example - micro_ros_arduino - micro-ros_publisher_wifi, and change parameters in set_microros_wifi_transports with your wifi, password and your host machine's ip address. Then run ros2 command: ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888
<img width="672" height="65" alt="image" src="https://github.com/user-attachments/assets/a5239db7-0dac-40e8-a9db-9b4fdf5ed93c" />

## 2. Hardware Setup
Build a two-wheel differential drive car with motors with encoders. Main parts include: ESP32-S3 chip, breadboard, L298N motor driver (or other motor driver), two DC motors with encoders, power:
   ![1](https://github.com/user-attachments/assets/11c653a8-bd1e-4746-ab47-9ea7a6c27048)
![3](https://github.com/user-attachments/assets/684d31fb-51c8-4095-a015-d56829a1c63f)
![2](https://github.com/user-attachments/assets/5e5d1bd2-8517-47df-8dda-366b81ed225b)

## 3. Code Explanation and How to Run...
1. micro-ros_publisher_wifi.ino - This is a demo code for verifying if you have connected the ESP32-S3 with your host machine, follow the software setup step #6.
2. motor_drive_test.ino - This is just a simple code to drive the car with ESP32-S3 only, without micro-ros.
3. control_car.ino - This code enables you to control the car using keyboard input from your host machine via micro-ROS and wifi. What you need to change are: 1. the motorpin/enablepin number according to your wire connection; 2. set_microros_wifi_transports to your wifi setting.

   After modification, upload this .ino to your ESP32-S3. Then, on your host machine, open the 1st terminal and connect micro-ROS with wifi
   <img width="1147" height="468" alt="image" src="https://github.com/user-attachments/assets/07344942-55d9-41c8-b559-3e44f729ddd6" />
   and then open the 2nd terminal, source the workspace, and then run the command: ros2 run teleop_twist_keyboard teleop_twist_keyboard
   <img width="822" height="503" alt="image" src="https://github.com/user-attachments/assets/4612ee4d-3943-486a-a955-ec9d9156273b" />
   At this point, if you stay in the 2nd terminal, you should be able to make the car go forward, backward, right, left and stop by using "i", ",", "j", "l" and "k" accordingly.
4. 

