#include <../include/main.h> // where tf is the cmakelists

Servo spigot_servo;
Spigot spigot(spigot_servo);

CRGB leds[NUM_LEDS];


void setup() {

  Serial.begin(115200);
  // ESP32PWM::allocateTimer(0);
  // spigot.init();
  // pinMode(LED_DATA_PIN, OUTPUT);
  FastLED.addLeds<WS2812, LED_DATA_PIN, BRG>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  FastLED.show();
  delay(1000);
  // Serial.println(comms::STARTUP_MSG);
}

void loop() {
  // digitalWrite(LED_DATA_PIN, HIGH);
  send(comms::MsgId::play_sound, "ON");
  for(auto &led : leds)
  {
    led = CRGB::Red;
  }

  FastLED.show();
  // spigot.open();
  delay(500);

  // digitalWrite(LED_DATA_PIN, LOW);

  send(comms::MsgId::play_sound, "OFF");
  for(auto &led : leds)
  {
    led = CRGB::Black;
  }
  FastLED.show();
  // spigot.close();
  delay(500);

}
