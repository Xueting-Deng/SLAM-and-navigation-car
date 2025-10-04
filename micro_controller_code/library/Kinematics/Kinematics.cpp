#include <Kinematics.h>
#include <Arduino.h>
//#include <vector>
//#include <tuple>

//std::vector<float> 
void Kinematics::kinematics_forward(float wheelspeed_1, float wheelspeed_2, float wheel_distance, float& linear_speed, float& angular_speed)
{
    
    linear_speed = (wheelspeed_1 + wheelspeed_2)/2.0;
    angular_speed = (wheelspeed_2 - wheelspeed_1)/wheel_distance;

}

void Kinematics::kinematics_inverse(float linear_speed, float angular_speed, float wheel_distance, float& out_wheelspeed_1, float& out_wheelspeed_2)
{
    out_wheelspeed_1 = linear_speed - (angular_speed*wheel_distance)/2.0;
    out_wheelspeed_2 = linear_speed + (angular_speed*wheel_distance)/2.0;
}

void Kinematics::update_odom(float car_linear_speed, float car_angular_speed, float dt, float& x, float& y, float& yaw)
{
    yaw = yaw + car_angular_speed * dt/1000;

    if (yaw > PI)
    {yaw = yaw - 2*PI;}
    else if (yaw < -PI)
    {yaw = yaw + 2*PI;}

    x = x + car_linear_speed*dt*cos(yaw)/1000; //car_linear_speed*dt = d_distance
    y = y + car_linear_speed*dt*sin(yaw)/1000;

}

void Kinematics::convert_quaternion(float roll, float pitch, float yaw, float& quaternion_x, float& quaternion_y, float& quaternion_z, float& quaternion_w)
{
    double cr = cos(roll * 0.5);
    double sr = sin(roll * 0.5);
    double cy = cos(yaw * 0.5);
    double sy = sin(yaw * 0.5);
    double cp = cos(pitch * 0.5);
    double sp = sin(pitch * 0.5);

    quaternion_w = cy * cp * cr + sy * sp * sr;
    
    quaternion_x = cy * cp * sr - sy * sp * cr;
    quaternion_y = sy * cp * sr + cy * sp * cr;
    quaternion_z = sy * cp * cr - cy * sp * sr;
}