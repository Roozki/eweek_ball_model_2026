#include <Arduino.h>
#include <ESP32Servo.h>


#include "pinout.h"

class Spigot {
public:
    Spigot(Servo& servo) : servo(servo) {};

    void init() {
      pinMode(SPIGOT_SERVO_PIN, OUTPUT);
        servo.setPeriodHertz(50);
        servo.attach(SPIGOT_SERVO_PIN);
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
    const uint32_t open_pos_us = 2000u;
    const uint32_t close_pos_us = 1000u;

};