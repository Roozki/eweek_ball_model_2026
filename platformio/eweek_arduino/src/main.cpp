#include <Arduino.h>
#include "../include/main.h"

#define SQUIDWARD_CUP_DETECTION_UPPER_THRESH_MM 120
#define SQUIDWARD_CUP_DETECTION_LOWER_THRESH_MM 45

#define PATRICK_CUP_DETECTION_UPPER_THRESH_MM 60
#define PATRICK_CUP_DETECTION_LOWER_THRESH_MM 0//50

#define SPONGEBOB_CUP_DETECTION_UPPER_THRESH_MM 60
#define SPONGEBOB_CUP_DETECTION_LOWER_THRESH_MM 0//50


// #define DEBUG_CUP_SENSE
#define bubble_up 140
#define bubble_down 0

uint32_t bubble_time_ms = 2000;
bool bubble_flag = false;

uint32_t last_bubble_timestamp_ms = 0;

Servo bubble_servo;

Servo spigot_servo;
Spigot spigot(spigot_servo, SPIGOT_SERVO_PIN);

void setup() {
  comms::init();
  // spigot.init();
spigot_servo.attach(9);
bubble_servo.attach(2);

  spigot.close();
  Serial.println("Arduino start");
  sonar_squidward_cup.init();
  sonar_patrick_cup.init();
  sonar_spongebob_cup.init();
  pinMode(PATRICK_HOUSE_LIMIT_SWITCH_PIN, INPUT_PULLUP);
  
}

void loop() {
  uint32_t timestamp_ms = millis();
  comms::poll_rx_buffer();

  float squidward_cup_distance = sonar_squidward_cup.find_distance_mm();
  float patrick_cup_distance = sonar_patrick_cup.find_distance_mm();
  float spongebob_cup_distance = sonar_spongebob_cup.find_distance_mm();
  if(squidward_cup_distance <= SQUIDWARD_CUP_DETECTION_UPPER_THRESH_MM && squidward_cup_distance >= SQUIDWARD_CUP_DETECTION_LOWER_THRESH_MM )
  {
    // comms::send(comms::MsgId::squidward_cup, "SQUID 1");
    comms::send(comms::MsgId::squidward_cup, "SQUID_1");
  } else {
    // comms::send(comms::MsgId::squidward_cup, "SQUID 0");
    comms::send(comms::MsgId::squidward_cup, "SQUID_0");
  }
  if(spongebob_cup_distance <= SPONGEBOB_CUP_DETECTION_UPPER_THRESH_MM && squidward_cup_distance >= SQUIDWARD_CUP_DETECTION_LOWER_THRESH_MM )
  {
    // comms::send(comms::MsgId::spongebob_cup, "SPONGE 1");
    comms::send(comms::MsgId::spongebob_cup, "SPONGE_1");
  } else {
    // comms::send(comms::MsgId::spongebob_cup, "SPONGE 0");
    comms::send(comms::MsgId::spongebob_cup, "SPONGE_0");
  }
  if(patrick_cup_distance <= PATRICK_CUP_DETECTION_UPPER_THRESH_MM && squidward_cup_distance >= SQUIDWARD_CUP_DETECTION_LOWER_THRESH_MM )
  {
    // comms::send(comms::MsgId::patrick_cup, "PATRICK 1");
    comms::send(comms::MsgId::patrick_cup, "PAT_1");
  } else {
    // comms::send(comms::MsgId::patrick_cup, "PATRICk 0");
    comms::send(comms::MsgId::patrick_cup, "PAT_0");
  }
// spigot_servo.writeMicroseconds(1300);

int patrick_house_state = digitalRead(PATRICK_HOUSE_LIMIT_SWITCH_PIN);
if(patrick_house_state == 0)
{
  comms::send(comms::MsgId::patrick_house_state, "PAT_HOUSE_1");
} else
{
  comms::send(comms::MsgId::patrick_house_state, "PAT_HOUSE_0");
}

// Serial.println(squidward_cup_distance);
delay(10);


if(timestamp_ms - last_bubble_timestamp_ms > bubble_time_ms)
{
  bubble_flag = !bubble_flag;
  if(bubble_flag)
  {
    bubble_servo.write(bubble_down);

  }else {
    bubble_servo.write(bubble_up);

  }
  
}



}
