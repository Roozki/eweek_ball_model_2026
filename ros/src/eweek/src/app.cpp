#include <app.h>

using namespace comms;

EWeekApp::EWeekApp() : Node("EWeek_App")
{
    auto qos = rclcpp::QoS(rclcpp::KeepLast(1)).reliable().durability_volatile(); //? AHHHH WHAT THE FUCK IS A QOS

    cmd_vel_subscriber = this->create_subscription<geometry_msgs::msg::Twist>(
        "/cmd_vel", 10, std::bind(&EWeekApp::CmdVelCallback, this, std::placeholders::_1));
    speaker_publisher = this->create_publisher<std_msgs::msg::String>(
        "/speaker/command", 10);

    bot_serial.setPort(bot_serial_port);
    bot_serial.setBaudrate(bot_serial_baud);
    bot_serial.open();
    
    structure_serial.setPort(structure_serial_port);
    structure_serial.setBaudrate(structure_serial_baud);
    structure_serial.open();
    double bot_period = 1.0 / BOT_RX_POLL_RATE;
    double structure_period = 1.0 / STRUCTURE_RX_POLL_RATE;
    double app_period = 1.0 / APP_RUN_RATE;

    bot_rx_timer_ = this->create_wall_timer(
    std::chrono::duration<double>(bot_period), std::bind(&EWeekApp::bot_rx, this));

    structure_rx_timer_ = this->create_wall_timer(
        std::chrono::duration<double>(structure_period), std::bind(&EWeekApp::structure_rx, this));

    app_timer = this->create_wall_timer(
        std::chrono::duration<double>(app_period), std::bind(&EWeekApp::run_app, this));

    sleep(0.1);
}


int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<EWeekApp>();

    RCLCPP_INFO(node->get_logger(), "Program init");

    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

void EWeekApp::run_app()
{
    char bot_msg[comms::MAX_MSG_SIZE_BYTES] = "";
    char structure_msg[comms::MAX_MSG_SIZE_BYTES] = "";

    switch(state){
        case State::startup:
            // if(bot_ir_state_back == IR_IN_MIDDLE)
            // {
                state = State::drive_to_patrick_start;
            // } //else, wait for ir state to be known
            RCLCPP_WARN(this->get_logger(), "STARTUP");
            break;
        case State::drive_to_patrick_start:
            RCLCPP_WARN(this->get_logger(), "DRIVE TO PATRICK");

            if(bot_ir_state_back == IR_IN_MIDDLE)
            {
                create_msg(comms::MsgId::drive_backward, bot_msg);
                bot_serial.write(bot_msg);
            } else if(bot_ir_state_back == IR_AT_END)
            {
                create_msg(comms::MsgId::drive_stop, bot_msg);
                bot_serial.write(bot_msg);
                // bot_serial.write(bot_msg); // double send for good measure
                state = State::await_patrick_open;
            }
            break;
        case State::await_patrick_open:
            RCLCPP_WARN(this->get_logger(), "AWAIT OPEN");

            if(patrick_house_state == PATRICK_HOUSE_OPEN)
            {
                state = State::await_patrick_cup;
            }
            break;
        case State::await_patrick_cup:
            RCLCPP_WARN(this->get_logger(), "AWAIT CUP");

            if(patrick_cup_state == CUP_PRESENT)
            {
                state = State::await_patrick_close;
            }
            break;
        case State::await_patrick_close:
            RCLCPP_WARN(this->get_logger(), "AWAIT PATRICK CLOSE");

            if(patrick_house_state == PATRICK_HOUSE_CLOSED)
            {
                sleep(WAIT_TIME_AFTER_PATRICK_CLOSE); // ahh so stupdi
                state = State::drive_to_squidward;
            }
            break;
        case State::drive_to_squidward:
            RCLCPP_WARN(this->get_logger(), "DRIVE TO SQUIDWARD");

            if(squidward_cup_state == CUP_NOT_PRESENT)
            {
                // RCLCPP_INFO(this->get_logger(), "DRIVE_TO_SQUIDWARD CUP NOT PRESENT");
                create_msg(comms::MsgId::drive_forward, bot_msg);
                bot_serial.write(bot_msg);
            } else if (squidward_cup_state == CUP_PRESENT)
            {

                // RCLCPP_INFO(this->get_logger(), "DRIVE_TO_SQUIDWARD CUP PRESENT");

                // sleep(SQUIDWARD_HYSTERIPUSSY_TIME_SECONDS);
                create_msg(comms::MsgId::drive_stop, bot_msg);
                // bot_serial.write(bot_msg);
                bot_serial.write(bot_msg);
                state = State::fill_cup;
            }
            break;
        case State::fill_cup:
                RCLCPP_INFO(this->get_logger(), "FILL CUP START");
            create_msg(comms::MsgId::drive_stop, bot_msg);
                // bot_serial.write(bot_msg);
                bot_serial.write(bot_msg);
                bot_serial.write(bot_msg);
                bot_serial.write(bot_msg);
            create_msg(comms::MsgId::open_spigot, structure_msg);
            structure_serial.write(structure_msg);
            structure_serial.write(structure_msg);
            memset(structure_msg, 0, comms::MAX_MSG_SIZE_BYTES); // wipe message
            sleep(CUP_FILL_TIME_SECONDS);
            create_msg(comms::MsgId::close_spigot, structure_msg);
            structure_serial.write(structure_msg);
            structure_serial.write(structure_msg);
            state = State::drive_to_sponge_end;

            break;
        case State::drive_to_sponge_end:
            RCLCPP_WARN(this->get_logger(), "DRIVE TO SPONGEBOB");
            // sleep(CUP_FILL_TIME_SECONDS);

            if(bot_ir_state_front == IR_IN_MIDDLE)
            {
                create_msg(comms::MsgId::drive_forward, bot_msg);
                bot_serial.write(bot_msg);
            }
            if(bot_ir_state_front == IR_AT_END && spongebob_cup_state == CUP_PRESENT) //! logic hook
            {
                sleep(SPONGEBOB_HYSTERIPUSSY_TIME_SECONDS);
                create_msg(comms::MsgId::drive_stop, bot_msg);
                bot_serial.write(bot_msg);
                state = State::await_user_take_cup;
            }
            break;
        case State::await_user_take_cup:
            if(spongebob_cup_state == CUP_NOT_PRESENT)
            {
                sleep(USER_CUP_TAKE_HYSTERISUSSY_TIME_SECONDS);
                state = State::drive_to_patrick_start;
            }
            break;       
        }
        
    RCLCPP_INFO(this->get_logger(), "IR_STATE_FRONT: %d", bot_ir_state_front);
    RCLCPP_INFO(this->get_logger(), "IR_STATE_BACK: %d", bot_ir_state_back);
    RCLCPP_INFO(this->get_logger(), "PATRICK_HOUSE: %d", patrick_house_state);
    RCLCPP_INFO(this->get_logger(), "PATRICK_CUP: %d", patrick_cup_state);
    RCLCPP_INFO(this->get_logger(), "SQUIDWARD_CUP: %d", squidward_cup_state);
    RCLCPP_INFO(this->get_logger(), "SPONGEBOB_CUP: %d", spongebob_cup_state);
}

void EWeekApp::CmdVelCallback(const geometry_msgs::msg::Twist::SharedPtr msg)
{
    char serial_msg[comms::MAX_MSG_SIZE_BYTES] = "";
    char structure_msg[comms::MAX_MSG_SIZE_BYTES] = "";
    if(msg->linear.x > 1)
    {
        create_msg(comms::MsgId::drive_forward, serial_msg);
        create_msg(comms::MsgId::open_spigot, structure_msg);
    } else if(msg->linear.x < -1)
    {
        create_msg(comms::MsgId::drive_backward, serial_msg);
        create_msg(comms::MsgId::close_spigot, structure_msg);

    } else {
        create_msg(comms::MsgId::drive_stop, serial_msg);
        create_msg(comms::MsgId::close_spigot, structure_msg);

    }
    structure_serial.write(structure_msg);
    bot_serial.write(serial_msg);
    return;
}


void EWeekApp::bot_rx()
{
    std::string next_char = "";
    std::string buffer = "";
    int timeoutCounter = 0;
   if (bot_serial.available() > 0){
       // ROS_WARN("Reading");

        //timeoutCounter ++;
       // next_char = teensy.read(); 
        buffer = bot_serial.read(RX_UART_BUFF_SIZE_BYTES);
        // RCLCPP_WARN(this->get_logger(), "%s", buffer.c_str());
        // if(next_char == "\n" || next_char == "\r" || next_char == "\0"){
        //     timeoutCounter = RX_UART_BUFF;
        // }
        if (buffer.size() > 0){
            if(buffer.find(static_cast<uint8_t>(comms::MsgId::comm_ping)) != std::string::npos){
                RCLCPP_INFO(this->get_logger(), "comms acked - bot");
                request_sound("bleep.wav");
                char msg[comms::MAX_MSG_SIZE_BYTES] = "";
                create_msg(comms::MsgId::comm_ack, msg);
                bot_serial.write(msg);
            }
            
            if(buffer.find(static_cast<uint8_t>(comms::MsgId::ir_state_front)) != std::string::npos){
                if(buffer.find("(IR_FRONT_1)") != std::string::npos)
                {
                    bot_ir_state_front = 1;
                }
                if(buffer.find("(IR_FRONT_0)") != std::string::npos)
                {
                    bot_ir_state_front = 0;
                }
            }

        if(buffer.find(static_cast<uint8_t>(comms::MsgId::ir_state_back)) != std::string::npos){
                if(buffer.find("(IR_BACK_1)") != std::string::npos)
                {
                    bot_ir_state_back = 1;
                }
                if(buffer.find("(IR_BACK_0)") != std::string::npos)
                {
                    bot_ir_state_back = 0;
                }
            }
        
        }
    }
}

void EWeekApp::structure_rx()
{
    std::string next_char = "";
    std::string buffer = "";
    int timeoutCounter = 0;
   if (structure_serial.available() > 0){
       // ROS_WARN("Reading");

        //timeoutCounter ++;
       // next_char = teensy.read(); 
        buffer = structure_serial.read(RX_UART_BUFF_SIZE_BYTES);
        // RCLCPP_WARN(this->get_logger(), "%s", buffer.c_str());
        // if(next_char == "\n" || next_char == "\r" || next_char == "\0"){
        //     timeoutCounter = RX_UART_BUFF;
        // }
        if (buffer.size() > 0){
            // Comm ack
            if(buffer.find(static_cast<uint8_t>(comms::MsgId::comm_ping)) != std::string::npos){
                RCLCPP_INFO(this->get_logger(), "comms acked - structure");
                request_sound("AI_engine_up.wav");
                char msg[comms::MAX_MSG_SIZE_BYTES] = "";
                create_msg(comms::MsgId::comm_ack, msg);
                structure_serial.write(msg);
                // return;
            }
            int msgid_pos = buffer.find(static_cast<uint8_t>(comms::MsgId::squidward_cup));
            if(msgid_pos != std::string::npos)
            {
                static bool flag = false;
                if(buffer.find("(SQUID_1)") != std::string::npos)
                {
                    // Squidward cup detected
                    squidward_cup_state = CUP_PRESENT;
                }

                if(buffer.find("(SQUID_0)") != std::string::npos)
                {
                    squidward_cup_state = CUP_NOT_PRESENT;
                }
                // else is a parsing error

            }

            msgid_pos = buffer.find(static_cast<uint8_t>(comms::MsgId::patrick_house_state));
            if(msgid_pos != std::string::npos)
            {
                // static bool flag = false;
                if(buffer.find("(PAT_HOUSE_1)") != std::string::npos)
                {
                    patrick_house_state = PATRICK_HOUSE_OPEN;
                }

                // Patrick house closed
                if(buffer.find("(PAT_HOUSE_0)") != std::string::npos)
                {
                    patrick_house_state = PATRICK_HOUSE_CLOSED;

                }
            }

            msgid_pos = buffer.find(static_cast<uint8_t>(comms::MsgId::patrick_cup));
            if(msgid_pos != std::string::npos)
            {
                if(buffer.find("(PAT_1)") != std::string::npos)
                {
                    patrick_cup_state = CUP_PRESENT;                    
                }

                // Patrick house closed
                if(buffer.find("(PAT_0)") != std::string::npos)
                {
                    patrick_cup_state = CUP_NOT_PRESENT;
                }
            }

            msgid_pos = buffer.find(static_cast<uint8_t>(comms::MsgId::spongebob_cup));
            if(msgid_pos != std::string::npos)
            {
                if(buffer.find("(SPONGE_1)") != std::string::npos)
                {
                    spongebob_cup_state = CUP_PRESENT;                    
                }

                // Patrick house closed
                if(buffer.find("(SPONGE_0)") != std::string::npos)
                {
                    spongebob_cup_state = CUP_NOT_PRESENT;
                }
            }
        }
    }
}

void EWeekApp::request_sound(std::string filename)
{
    std_msgs::msg::String msg;
    msg.data = filename;
    speaker_publisher->publish(msg);
}