#include <Arduino.h>
#include "../include/main.h"

#define SQUIDWARD_CUP_DETECTION_UPPER_THRESH_MM 60
#define SQUIDWARD_CUP_DETECTION_LOWER_THRESH_MM 50

Servo spigot_servo;
Spigot spigot(spigot_servo, SPIGOT_SERVO_PIN);

void setup() {
  comms::init();
  // spigot.init();
spigot_servo.attach(9);

  spigot.close();
  Serial.println("Arduino start");
  sonar_squidward_cup.init();
  
}

void loop() {
  uint32_t timestamp_ms = millis();
  comms::poll_rx_buffer();

  float squidward_cup_distance = sonar_squidward_cup.find_distance_mm();
  if(squidward_cup_distance <= SQUIDWARD_CUP_DETECTION_UPPER_THRESH_MM && squidward_cup_distance >= SQUIDWARD_CUP_DETECTION_LOWER_THRESH_MM )
  {
    comms::send(comms::MsgId::squidward_cup, "1");
  } else {
    comms::send(comms::MsgId::squidward_cup, "0");
  }
// spigot_servo.writeMicroseconds(1300);



// Serial.println(squidward_cup_distance);
delay(50);



}
