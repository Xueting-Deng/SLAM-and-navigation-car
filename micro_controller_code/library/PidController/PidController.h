#ifndef __PIDCONTROLLER_H__
#define __PIDCONTROLLER_H__

class PidController 
{
    public:
        PidController() = default;
        PidController(float kp, float ki, float kd);

    public:
        float target_;
        float out_min_;
        float out_max_;
        float kp_;
        float ki_;
        float kd_;
        float last_output_;

        float error_sum_;
        float derror_;
        float error_pre_;
        float error_last_;
        float intergral_up_ = 2500;

    public:
        float update(float control);
        void reset();
        void update_pid(float kp, float ki, float kd);
        void update_target(float target);
        void out_limit(float out_min, float out_max);
};

#endif
