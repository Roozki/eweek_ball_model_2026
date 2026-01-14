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
#include "state_machine.h"