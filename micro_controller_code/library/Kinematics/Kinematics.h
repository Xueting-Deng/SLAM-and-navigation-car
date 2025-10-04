#ifndef __KINEMATICS_H__
#define __KINEMATICS_H__

// typedef struct odom_t
// {
//     float x;
//     float y;
//     float yaw;
//     float linear_speed;
//     float angular_speed;
// };



class Kinematics
{
private:
    /* data */
public:
    Kinematics() = default;
    void kinematics_forward(float wheelspeed_1, float wheelspeed_2, float wheel_distance, float& linear_speed, float& angular_speed);
    void kinematics_inverse(float linear_speed, float angular_speed, float wheel_distance, float& out_wheelspeed_1, float& out_wheelspeed_2);
    //float update_odom();
    void update_odom(float car_linear_speed, float car_angular_speed, float dt, float& x, float& y, float& yaw);
    void convert_quaternion(float roll, float pitch, float yaw, float& quaternion_x, float& quaternion_y, float& quaternion_z, float& quaternion_w);

};


#endif