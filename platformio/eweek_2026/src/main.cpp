#include <../include/main.h> // where tf is the cmakelists

using namespace comms;

int poll_ir_front();
int poll_ir_back();

int prev_ir_front_state = 0;
int prev_ir_back_state = 0;


void setup() {

  // Serial.begin(115200);
  comms::init();
  // ESP32PWM::allocateTimer(0);
  // spigot.init();
  // pinMode(LED_DATA_PIN, OUTPUT);
  machine.init();
  drive.init();
  drive.setSpeed(30);
  drive.setAccel(50);
  // FastLED.addLeds<WS2812, LED_DATA_PIN, BRG>(leds, NUM_LEDS);
  // FastLED.setBrightness(50);
  // FastLED.show();

  pinMode(IR_DATA_PIN_FRONT, INPUT_PULLUP);
  pinMode(IR_DATA_PIN_BACK, INPUT_PULLUP);


  delay(1);
  // Serial.println(comms::STARTUP_MSG);
}

void run_app()
{

}

void loop() {

  uint32_t timestamp_ms = millis();  
  comms::poll_rx_buffer();
  drive.run(timestamp_ms);
  int ir_state_front = poll_ir_front(); 
  int ir_state_back = poll_ir_back(); 
  // stupid easy sending
  if(ir_state_front == 1)
  {
    comms::send(MsgId::ir_state_front, "IR_FRONT_1");
  } else 
  {
    comms::send(MsgId::ir_state_front, "IR_FRONT_0");
    if(ir_state_front != prev_ir_front_state)
    {
     
      // drive.setState(DriveState::stop);

      // delay(500);
    }
  }
      prev_ir_front_state = ir_state_front;

  if(ir_state_back == 1)
  {
    comms::send(MsgId::ir_state_back, "IR_BACK_1");
  } else 
  {
    comms::send(MsgId::ir_state_back, "IR_BACK_0");
        if(ir_state_back != prev_ir_back_state)
    {
      // drive.current_A_speed = 0;
      // drive.current_B_speed = 0;
      // drive.run(timestamp_ms);
      // drive.setState(DriveState::stop);
      // drive.run(timestamp_mse/)
      // delay(500);
    }
  }

      prev_ir_back_state = ir_state_back;

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
                // digitalWrite(RIGHT_A_IN1_PIN, HIGH);
                // digitalWrite(RIGHT_A_IN2_PIN, LOW);
          // analogWrite(drive.A_pwm_pin, drive.current_A_speed);
        // analogWrite(drive.B_pwm_pin, drive.current_B_speed);

  // Serial.println(drive.current_A_speed);

  delay(10);
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



int poll_ir_front()
{
 return digitalRead(IR_DATA_PIN_FRONT);

}
int poll_ir_back()
{
 return digitalRead(IR_DATA_PIN_BACK);

}