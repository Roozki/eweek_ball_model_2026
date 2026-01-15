#include <../include/main.h> // where tf is the cmakelists

using namespace comms;

int poll_ir();

void setup() {

  // Serial.begin(115200);
  comms::init();
  // ESP32PWM::allocateTimer(0);
  // spigot.init();
  // pinMode(LED_DATA_PIN, OUTPUT);
  machine.init();
  drive.init();
  drive.setSpeed(25);
  drive.setAccel(40);
  // FastLED.addLeds<WS2812, LED_DATA_PIN, BRG>(leds, NUM_LEDS);
  // FastLED.setBrightness(50);
  // FastLED.show();

  pinMode(IR_DATA_PIN, INPUT);

  delay(100);
  // Serial.println(comms::STARTUP_MSG);
}

void run_app()
{

}

void loop() {

  uint32_t timestamp_ms = millis();
  comms::poll_rx_buffer();
  int ir_state = poll_ir(); 
  // stupid easy sending
  if(ir_state == 1)
  {
    comms::send(MsgId::ir_state, "A");
  } else 
  {
    comms::send(MsgId::ir_state, "B");
  }
  // digitalWrite(LED_DATA_PIN, HIGH);
  // send(comms::MsgId::play_sound, "ON");
  // for(auto &led : leds)
  // {
  //   led = CRGB::Red;
  // }
  if(machine.get_state() == State::await_comms_ack)
  {
    if(timestamp_ms - machine.last_comm_ack_msg_timestamp_ms >= machine.comm_ack_msg_period_ms)
    {
      // comms::send(MsgId::comm_ping);
      machine.last_comm_ack_msg_timestamp_ms = timestamp_ms;
    }
  }

  // drive.setSpeed(30);
  //   if(timestamp_ms > 5000)
  // {
  //   drive.setState(DriveState::stop);
  //   if(timestamp_ms > 10000)
  //   {
  //     drive.setState(DriveState::backward);
  //     if (timestamp_ms > 15000)
  //     {
  //       drive.setState(DriveState::stop);
  //     }
  //   }
  // } else {
  //   drive.setState(DriveState::forward);
  // }
  // drive.current_A_speed = 80;
  // drive.current_B_speed = 80;
  drive.run(timestamp_ms);
                // digitalWrite(RIGHT_A_IN1_PIN, HIGH);
                // digitalWrite(RIGHT_A_IN2_PIN, LOW);
          // analogWrite(drive.A_pwm_pin, drive.current_A_speed);
        // analogWrite(drive.B_pwm_pin, drive.current_B_speed);

  // Serial.println(drive.current_A_speed);

  delay(1);
  // FastLED.show();
  // // spigot.open();
  // delay(500);

  // // digitalWrite(LED_DATA_PIN, LOW);

  // send(comms::MsgId::play_sound, "OFF");
  // for(auto &led : leds)
  // {
  //   led = CRGB::Black;
  // }
  // FastLED.show();
  // // spigot.close();
  // delay(500);

}



int poll_ir()
{
 return digitalRead(IR_DATA_PIN);

}