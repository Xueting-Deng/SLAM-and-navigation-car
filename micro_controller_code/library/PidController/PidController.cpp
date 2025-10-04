#include "PidController.h"
#include "Arduino.h"

PidController::PidController(float kp, float ki, float kd)
{
    reset();
    update_pid(kp, ki, kd);
}

void PidController::reset()
{
    target_ = 0.0f;
    out_min_ = 0.0f;
    out_max_ = 0.0f;
    kp_ = 0.0f;
    ki_ = 0.0f;
    kd_ = 0.0f;
    last_output_ = 0.0f;

    error_sum_ = 0.0f;
    derror_ = 0.0f;
    error_pre_ = 0.0f;
    error_last_ = 0.0f;
}

void PidController::update_pid(float kp, float ki, float kd)
{
    reset();
    kp_ = kp;
    ki_ = ki;
    kd_ = kd;
}

void PidController::update_target(float target)
{
    target_ = target;
}

void PidController::out_limit(float out_min, float out_max)
{
    out_min_ = out_min;
    out_max_ = out_max;
}

float PidController::update(float control)
{
    float error = target_ - control; //difference
    derror_ = error_last_ - error; //derivation
    error_sum_ = error_sum_ + error; //intergral

    // limit the integral
    if (error_sum_ > intergral_up_)
        error_sum_ = intergral_up_;
    if (error_sum_ < -1*intergral_up_)
        error_sum_ = -1*intergral_up_;

    float output = kp_*error + ki_*error_sum_ + kd_*derror_;
    
    // limit the output
    if (output > out_max_)
        output = out_max_;
    if (output < out_min_)
        output = out_min_;

    error_last_ = error; //update last error
    last_output_ = output; //update last output

    return output;
}