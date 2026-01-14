#pragma once
#include <ESP32Servo.h>
#include <spigot.h>
#include <FastLED.h>
#include <drive.h>
#include "state_machine.h"

Servo spigot_servo;
Spigot spigot(spigot_servo);
L298n_Drive drive(RIGHT_A_DRIVE_PWM_PIN, RIGHT_A_IN1_PIN, RIGHT_A_IN2_PIN, LEFT_B_DRIVE_PWM_PIN, LEFT_B_IN3_PIN, LEFT_B_IN4_PIN);

CRGB leds[NUM_LEDS];

StateMachine machine;