#pragma once
#ifdef esp
#include <ESP32Servo.h>
#include "pinout_esp.h"
#endif

#ifdef arduino
#include <Servo.h>
#include "pinout_arduino.h"
#endif


#include "spigot.h"
#include <FastLED.h>
#include "drive.h"
#include "state_machine.h"
#include "sonar.h"

#ifdef esp
inline Servo spigot_servo;
inline Spigot spigot(spigot_servo, 0);
#endif

#ifdef arduino
inline Sonar sonar_squidward_cup(SONIC_SQUIDWARD_ECHO_PIN, SONIC_SQUIDWARD_TRIG_PIN);
inline Sonar sonar_patrick_cup(SONIC_PATRICK_ECHO_PIN, SONIC_PATRICK_TRIG_PIN);
inline Sonar sonar_spongebob_cup(SONIC_SPONGEBOB_ECHO_PIN, SONIC_SPONGEBOB_TRIG_PIN);

// extern Servo spigot_servo;
extern Spigot spigot;
#endif

L298n_Drive drive(RIGHT_A_DRIVE_PWM_PIN, RIGHT_A_IN1_PIN, RIGHT_A_IN2_PIN, LEFT_B_DRIVE_PWM_PIN, LEFT_B_IN3_PIN, LEFT_B_IN4_PIN);

CRGB leds[NUM_LEDS];

StateMachine machine;