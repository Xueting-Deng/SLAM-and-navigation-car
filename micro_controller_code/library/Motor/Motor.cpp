#include "Motor.h"
#include "Arduino.h"


// Motor* Motor::instance_[2] = {nullptr, nullptr};

Motor::Motor(int motorPin1, int motorPin2, int motorEnable, int freq, int resolution, int pwmChannel)
{
    set_pin(motorPin1, motorPin2, motorEnable);
    set_pwm(freq, resolution, pwmChannel);
}

void Motor::set_pin(int motorPin1, int motorPin2, int motorEnable)
{
    motorPin1_ = motorPin1;
    motorPin2_ = motorPin2;
    motorEnable_ = motorEnable;

    pinMode(motorPin1_, OUTPUT);
    pinMode(motorPin2_, OUTPUT);
    pinMode(motorEnable_, OUTPUT);
}

void Motor::set_pwm(int freq, int resolution, int pwmChannel)
{
    freq_ = freq;
    resolution_ = resolution;
    pwmChannel_ = pwmChannel;
}

void Motor::motor_on()
{  
    ledcAttachChannel(motorEnable_, freq_, resolution_, pwmChannel_);
}

void Motor::motor_off()
{  
    digitalWrite(motorPin1_, LOW);
    digitalWrite(motorPin2_, LOW);
}

void Motor::motor_forward()
{  
    digitalWrite(motorPin1_, LOW);
    digitalWrite(motorPin2_, HIGH);
}

void Motor::motor_backward()
{  
    digitalWrite(motorPin1_, HIGH);
    digitalWrite(motorPin2_, LOW);
}

void Motor::motor_dutycycle_update(int dutyCycle)
{
    ledcWrite(motorEnable_, dutyCycle);
}

// void Motor::encoder_on()
// {
//     //instance_ = this;
//     attachInterrupt(digitalPinToInterrupt(encoderPin1_), encoder_update, RISING);
// }

// void Motor::encoder_on(int isr_id) {
//     if (isr_id < 2) {
//         instance_[isr_id] = this;
//         if (isr_id == 0) {
//             attachInterrupt(digitalPinToInterrupt(encoderPin1_), encoder_isr_0, RISING);
//         } else if (isr_id == 1) {
//             attachInterrupt(digitalPinToInterrupt(encoderPin1_), encoder_isr_1, RISING);
//         }
//     }
// }

// void Motor::encoder_isr_0() {
//     if (instance_[0]) instance_[0]->encoder_update();
// }
// void Motor::encoder_isr_1() {
//     if (instance_[1]) instance_[1]->encoder_update();
// }

// void Motor::encoder_update()
// {
//   if (digitalRead(encoderPin1_) > digitalRead(encoderPin2_))
//   Encodervalue_ = Encodervalue_ - 1;
//   else
//   Encodervalue_ = Encodervalue_ + 1;
// }

// long Motor::get_encoder_value() const {
//     return Encodervalue_;
// }


