#pragma once
#include <Arduino.h>

#ifdef esp
#include <ESP32Servo.h>
#endif

#ifdef arduino
#include <Servo.h>
#endif

class Spigot {
public:
    Spigot(Servo& servo, int pin) : servo(servo), pin(pin) {};

    void init() {
    //   pinMode(pin, OUTPUT);
      #ifdef esp
        servo.setPeriodHertz(50);
    #endif
        servo.attach(pin);
    }

    void open() {
        servo.writeMicroseconds(open_pos_us);
    }

    void close()
    {
        servo.writeMicroseconds(close_pos_us);
    }

private:
    Servo& servo;
    int pin = 0;
    const uint32_t open_pos_us = 1500u;
    const uint32_t close_pos_us = 920u;
    //Hitech HS 55 - 900 -> 2100us . 1500us center

};