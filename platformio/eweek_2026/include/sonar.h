#pragma once
#include <Arduino.h>


#include "pinout_esp.h"

// void ARDUINO_ISR_ATTR front_sonar_isr() {

// }

// static inline constexpr float SPEED_SOUND_MPS = 343;



class Sonar {
public:
    Sonar(int echo_pin, int trig_pin) : echo_pin(echo_pin), trig_pin(trig_pin) {};

    void init() {
        pinMode(echo_pin, INPUT);
        pinMode(trig_pin, OUTPUT);
        digitalWrite(trig_pin, LOW);
    }

    float find_distance_mm()
    {
        reset();
        start_wave();
        float duration = await_wave();
        float distance_mm = (duration*.0343*10)/((float)2);
        return distance_mm;
    }

private:
    int echo_pin;
    int trig_pin;


    void start_wave()
    {
        digitalWrite(trig_pin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trig_pin, LOW);
    }
    float await_wave()
    {
       return pulseIn(echo_pin, HIGH, 100000);
    }

    void reset()
    {
        digitalWrite(trig_pin, LOW);
        delayMicroseconds(2); // Reset
    }

};