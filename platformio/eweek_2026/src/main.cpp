#include <../include/main.h> // where tf is the cmakelists

Servo spigot_servo;
Spigot spigot(spigot_servo);

void setup() {

  Serial.begin(115200);
  ESP32PWM::allocateTimer(0);
  spigot.init();

  delay(1000);
  Serial.println(comms::STARTUP_MSG);

}

void loop() {
  // put your main code here, to run repeatedly:
  // send(comms::MsgId::play_sound);
  spigot.open();
  delay(1000);
  spigot.close();
  delay(1000);


}
