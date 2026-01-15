#pragma once
#include <Arduino.h>
#include "main_definitions.h"

#define MAX_PWM_RAW_OUTPUT 200
#define MIN_PWM_RAW_OUTPUT 0

enum class DriveState {
    stop, 
    forward,
    backward
};  

// Should be split into 2 classes - one for motor, one for differential drive.
class L298n_Drive {
public:
    L298n_Drive (uint32_t A_pwm_pin, uint32_t in1_pin, uint32_t in2_pin, uint32_t B_pwm_pin, uint32_t in3_pin, uint32_t in4_pin)
    : A_pwm_pin(A_pwm_pin),
    in1_pin(in1_pin),
    in2_pin(in2_pin),
    B_pwm_pin(B_pwm_pin),
    in3_pin(in3_pin),
    in4_pin(in4_pin) {};


    void init() {
        // ledcAttachChannel(A_pwm_pin, PWM_FREQ, PWM_RES, RIGHT_PWM_CH);
        #ifdef esp
        ledcSetup(RIGHT_PWM_CH, PWM_FREQ, PWM_RES);
        ledcAttachPin(A_pwm_pin, RIGHT_PWM_CH);

        ledcSetup(LEFT_PWM_CH, PWM_FREQ, PWM_RES);
        ledcAttachPin(B_pwm_pin, LEFT_PWM_CH);

        // pinMode(A_pwm_pin, OUTPUT);
        pinMode(in1_pin, OUTPUT);
        pinMode(in2_pin, OUTPUT);
        // pinMode(B_pwm_pin, OUTPUT);
        pinMode(in3_pin, OUTPUT);
        pinMode(in4_pin, OUTPUT);
        digitalWrite(in1_pin, LOW);
        digitalWrite(in2_pin, LOW);
        digitalWrite(in3_pin, LOW);
        digitalWrite(in4_pin, LOW);
        // analogWrite(A_pwm_pin, 0);
        // analogWrite(B_pwm_pin, 0);
        #endif
    }

    void run(uint32_t timestamp_ms = 0) // Must be called regularily to deal with accelleration
    {
        #ifdef esp
        ledcWrite(RIGHT_PWM_CH, current_A_speed);
        ledcWrite(LEFT_PWM_CH, current_B_speed);
        // analogWrite(A_pwm_pin, current_A_speed);
        // analogWrite(B_pwm_pin, current_B_speed);

        // if(current_A_speed == 0)
        // {
        //         digitalWrite(in1_pin, LOW);
        //         digitalWrite(in2_pin, LOW);
        // }
        // if(current_B_speed == 0)
        // {
        //     digitalWrite(in3_pin, LOW);
        //     digitalWrite(in4_pin, LOW);
        // }
        if(timestamp_ms - this->last_run_timestamp_ms >= this->update_period_ms)
        {
            handle_accel(current_A_speed, desired_A_speed);
            handle_accel(current_B_speed, desired_B_speed);
            this->last_run_timestamp_ms = timestamp_ms;
        }
        #endif
    }

    void setSpeed(int32_t speed)
    {
        int32_t new_speed = map(speed, 0, 100, MIN_PWM_RAW_OUTPUT, MAX_PWM_RAW_OUTPUT);
        setpoint_speed = new_speed;
    }

    // Like set gear
    void setState(DriveState state) 
    {
        #ifdef esp
        this->current_drive_state = state;

        switch(current_drive_state){
            case DriveState::stop:
                // setSpeed(0);
                this->desired_A_speed = 0;
                this->desired_B_speed = 0;
                // Allow accel to stop motors
                break;
            case DriveState::forward:
                this->desired_A_speed = setpoint_speed;
                this->desired_B_speed = setpoint_speed;

                digitalWrite(in1_pin, LOW);
                digitalWrite(in2_pin, HIGH);

                digitalWrite(in3_pin, LOW);
                digitalWrite(in4_pin, HIGH);
                break;
            case DriveState::backward:
                this->desired_A_speed = setpoint_speed;
                this->desired_B_speed = setpoint_speed;
                digitalWrite(in1_pin, HIGH);
                digitalWrite(in2_pin, LOW);

                digitalWrite(in3_pin, HIGH);
                digitalWrite(in4_pin, LOW);
                break;


        }
        #endif
    }

    void setAccel(uint32_t accel)
    {
        this->max_accel = accel;
    }

    void setUpdatePeriodMs(uint32_t period_ms)
    {
        this->update_period_ms = period_ms;
    }

    bool isStopped()
    {
        if(current_A_speed == 0 && current_B_speed == 0)
        {
            return true;
        }
        return false;
    }

    
// private:
    // A motor
    uint32_t A_pwm_pin = 0;
    uint32_t in1_pin = 0;
    uint32_t in2_pin = 0;

    // B motor
    uint32_t B_pwm_pin = 0;
    uint32_t in3_pin = 0;
    uint32_t in4_pin = 0;

    // Other characteristics
    uint32_t max_accel = 1; // percent / s

    DriveState current_drive_state = DriveState::stop;

    uint32_t last_run_timestamp_ms = 0;
    int32_t current_A_speed = 0;
    int32_t desired_A_speed = 0;

    int32_t current_B_speed = 0;
    int32_t desired_B_speed = 0;

    uint32_t update_period_ms = 10; // Updates at 100hz default

    int32_t setpoint_speed = 30;

    void handle_accel(int32_t& current_speed, int32_t desired_speed)
    {
        // accell
        if(current_speed < desired_speed)
        {
            current_speed += max_accel;
            if(current_speed > desired_speed){
                current_speed = desired_speed;
            }
        }

        // decel
        if(current_speed > desired_speed)
        {
            current_speed -= max_accel;
            if(current_speed < desired_speed){
                current_speed = desired_speed;
            }
        }


        // if(current_speed - this->max_accel > desired_speed)
        // {
        //     current_speed -= this->max_accel;
        // } else {
        //     current_speed = desired_speed;
        // }

        // // Accel
        // if(current_speed + this->max_accel < desired_speed)
        // {
        //     current_speed += this->max_accel;
        // } else {
        //     current_speed = desired_speed;
        // }
    }

};