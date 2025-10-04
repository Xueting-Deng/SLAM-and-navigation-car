#ifndef __MOTOR_H__
#define __MOTOR_H__

class Motor
{
    public:
        Motor() = default;
        Motor(int motorPin1, int motorPin2, int motorEnable, int freq, int resolution, int pwmChannel);

    public:
        // motor and encoder pin
        int motorPin1_;
        int motorPin2_;
        int motorEnable_;


        // pwm parameter
        int freq_;
        int resolution_;
        int pwmChannel_;

        volatile long Encodervalue_ = 0;

    public:
        //void reset();
        void set_pin(int motorPin1, int motorPin2, int motorEnable);
        void set_pwm(int freq, int resolution, int pwmChannel);
        void motor_on();
        void motor_off();
        void motor_forward();
        void motor_backward();
        void motor_dutycycle_update(int dutyCycle);

    //     static void encoder_isr_0();
    //     static void encoder_isr_1();
    //     void encoder_on(int isr_id);
    //     void encoder_update();
    //     long get_encoder_value() const;

    // private:
    // static Motor* instance_[2];
};

#endif
