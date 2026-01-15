#pragma once
// Pins for gpio, comms, etc
// No comms pins (automatically from arduino)

// ARDUINO 


// Ultrasonic Front sensor
#define SONIC_SQUIDWARD_TRIG_PIN 13
#define SONIC_SQUIDWARD_ECHO_PIN 12

// Servo for drink spigot
//Default: ±60º, Non-programmable / Pulse Width: 900~2100μs(Center:1500μs) HS-55 hitec
#define SPIGOT_SERVO_PIN 9


// LEDS
#define LED_DATA_PIN 2


// DRIVE (none for arduino)
#define RIGHT_A_DRIVE_PWM_PIN 0
#define RIGHT_A_IN1_PIN 0
#define RIGHT_A_IN2_PIN 0

#define LEFT_B_DRIVE_PWM_PIN 0
#define LEFT_B_IN3_PIN 0
#define LEFT_B_IN4_PIN 0