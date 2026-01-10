#pragma once
#include <Arduino.h>


#include "pinout.h"

// void ARDUINO_ISR_ATTR front_sonar_isr() {

// }

static inline constexpr float SPEED_SOUND_MPS = 343;



class Sonar {
public:
    Sonar() {};

    void init() {
        pinMode(SONIC_FRONT_ECHO_PIN, INPUT);
        pinMode(SONIC_FRONT_TRIG_PIN, OUTPUT);
        digitalWrite(SONIC_FRONT_TRIG_PIN, LOW);
    }

    uint32_t get_distance()
    {
        start_wave();
    }

private:

    void start_wave()
    {
        digitalWrite(SONIC_FRONT_TRIG_PIN, HIGH);
        delayMicroseconds(10);

    }

    void reset()
    {
        digitalWrite(SONIC_FRONT_TRIG_PIN, LOW);
        delayMicroseconds(2); // Reset
    }

    // void trig_interrupt()
    // {

    // }

};