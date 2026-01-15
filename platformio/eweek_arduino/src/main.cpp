#include <Arduino.h>
#include "../include/main.h"
#include <Arduino_FreeRTOS.h>
#include <FastLED.h>

#define SQUIDWARD_CUP_DETECTION_UPPER_THRESH_MM 60
#define SQUIDWARD_CUP_DETECTION_LOWER_THRESH_MM 50

#define PATRICK_CUP_DETECTION_UPPER_THRESH_MM 60
#define PATRICK_CUP_DETECTION_LOWER_THRESH_MM 0//50

#define SPONGEBOB_CUP_DETECTION_UPPER_THRESH_MM 60
#define SPONGEBOB_CUP_DETECTION_LOWER_THRESH_MM 0//50

#define NUM_LEDS 120
#define LED_DATA_PIN 12

CRGB leds[NUM_LEDS];

Servo spigot_servo;
Spigot spigot(spigot_servo, SPIGOT_SERVO_PIN);

void setup() {
  comms::init();
  // spigot.init();
spigot_servo.attach(9);

  spigot.close();
  Serial.println("Arduino start");
  sonar_squidward_cup.init();
  sonar_patrick_cup.init();
  sonar_spongebob_cup.init();
  pinMode(PATRICK_HOUSE_LIMIT_SWITCH_PIN, INPUT_PULLUP);
  
  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500); 
}

void pattern_someone_is_home() {
  // 1. Pick a base warm color (Orange-ish)
  uint8_t baseHue = 35; 
  
  // 2. Add randomness to simulate flickering light
  // 'random8(40)' adds a small variation to brightness so it's not static
  // 'beatsin8' adds a slower undertone so it's not too erratic
  uint8_t flicker = random8(40);
  uint8_t wave = beatsin8(10, 0, 40);
  
  // 200 is the "base" brightness. We subtract flicker/wave to make it dip down.
  uint8_t brightness = 255 - flicker - wave;

  // 3. Occasionally shift the Hue slightly to simulate shifting light warmth
  uint8_t hue_shift = random8(5);

  fill_solid(leds, NUM_LEDS, CHSV(baseHue + hue_shift, 255, brightness));
  
  FastLED.show();
}

void loop() {
  uint32_t timestamp_ms = millis();
  comms::poll_rx_buffer();
  pattern_someone_is_home();
  float squidward_cup_distance = sonar_squidward_cup.find_distance_mm();
  float patrick_cup_distance = sonar_patrick_cup.find_distance_mm();
  float spongebob_cup_distance = sonar_spongebob_cup.find_distance_mm();
  if(squidward_cup_distance <= SQUIDWARD_CUP_DETECTION_UPPER_THRESH_MM && squidward_cup_distance >= SQUIDWARD_CUP_DETECTION_LOWER_THRESH_MM )
  {
    // comms::send(comms::MsgId::squidward_cup, "SQUID 1");
    comms::send(comms::MsgId::squidward_cup, "1");
  } else {
    // comms::send(comms::MsgId::squidward_cup, "SQUID 0");
    comms::send(comms::MsgId::squidward_cup, "0");
  }
  if(spongebob_cup_distance <= SPONGEBOB_CUP_DETECTION_UPPER_THRESH_MM && squidward_cup_distance >= SQUIDWARD_CUP_DETECTION_LOWER_THRESH_MM )
  {
    // comms::send(comms::MsgId::spongebob_cup, "SPONGE 1");
    comms::send(comms::MsgId::spongebob_cup, "1");
  } else {
    // comms::send(comms::MsgId::spongebob_cup, "SPONGE 0");
    comms::send(comms::MsgId::spongebob_cup, "0");
  }
  if(patrick_cup_distance <= PATRICK_CUP_DETECTION_UPPER_THRESH_MM && squidward_cup_distance >= SQUIDWARD_CUP_DETECTION_LOWER_THRESH_MM )
  {
    // comms::send(comms::MsgId::patrick_cup, "PATRICK 1");
    comms::send(comms::MsgId::patrick_cup, "1");
  } else {
    // comms::send(comms::MsgId::patrick_cup, "PATRICk 0");
    comms::send(comms::MsgId::patrick_cup, "0");
  }
// spigot_servo.writeMicroseconds(1300);

int patrick_house_state = digitalRead(PATRICK_HOUSE_LIMIT_SWITCH_PIN);
if(patrick_house_state == 0)
{
  comms::send(comms::MsgId::patrick_house_state, "1");
} else
{
  comms::send(comms::MsgId::patrick_house_state, "0");
}

// Serial.println(squidward_cup_distance);
delay(50);



}
