#pragma once
#define esp
// #define ardunio

// External includes
#include <Arduino.h>

// target specific libs
#ifdef arduino
    #include <Servo.h>
#endif

#ifdef esp
    #include <ESP32Servo.h>
    #include "pinout_esp.h"
#endif

// #define FASTLED_ESP32_I2S true

// Internal Includes
#include "main_definitions.h"
#include "comms.h"
#include "spigot.h"
#include "sonar.h"
#include "drive.h"
#include "main_datatypes.h"
// #include

// Servo spigot_servo;
// Spigot spigot(spigot_servo);
// L298n_Drive drive(RIGHT_A_DRIVE_PWM_PIN, RIGHT_A_IN1_PIN, RIGHT_A_IN2_PIN, LEFT_B_DRIVE_PWM_PIN, LEFT_B_IN3_PIN, LEFT_B_IN4_PIN);

// CRGB leds[NUM_LEDS];

