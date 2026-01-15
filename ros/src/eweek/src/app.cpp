#include <app.h>
#include <filesystem>
#include <iostream>
#include <ament_index_cpp/get_package_share_directory.hpp>

using namespace comms;

EWeekApp::EWeekApp() : Node("EWeek_App")
{
    // Load sounds dynamically
    try {
        std::string sounds_dir = ament_index_cpp::get_package_share_directory("rover_sounds") + "/sounds";
        if(std::filesystem::exists(sounds_dir) && std::filesystem::is_directory(sounds_dir)) {
            for(const auto& entry : std::filesystem::directory_iterator(sounds_dir)) {
                if(entry.is_regular_file()) {
                    std::string ext = entry.path().extension().string();
                    if(ext == ".wav" || ext == ".ogg" || ext == ".mp3") { // Supporting mp3 if we added support earlier or just listing
                         // Store just the filename as the path is known by the speaker node
                        sound_files.push_back(entry.path().filename().string());
                    }
                }
            }
            RCLCPP_INFO(this->get_logger(), "Loaded %zu sound files", sound_files.size());
        } else {
             RCLCPP_WARN(this->get_logger(), "Sound directory not found: %s", sounds_dir.c_str());
        }
    } catch (const std::exception& e) {
        RCLCPP_ERROR(this->get_logger(), "Error loading sounds: %s", e.what());
    }

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
    double app_period = 1.0 / 50;

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
            if(bot_ir_state == IR_IN_MIDDLE)
            {
                state = State::drive_to_patrick_start;
            } //else, wait for ir state to be known
            break;
        case State::drive_to_patrick_start:
            if(bot_ir_state == IR_IN_MIDDLE)
            {
                create_msg(comms::MsgId::drive_backward, bot_msg);
                bot_serial.write(bot_msg);
            } else if(bot_ir_state == IR_AT_END)
            {
                create_msg(comms::MsgId::drive_stop, bot_msg);
                bot_serial.write(bot_msg);
                // bot_serial.write(bot_msg); // double send for good measure
                state = State::await_patrick_open;
            }
            break;
        case State::await_patrick_open:
            if(patrick_house_state == PATRICK_HOUSE_OPEN)
            {
                state = State::await_patrick_cup;
            }
            break;
        case State::await_patrick_cup:
            if(patrick_cup_state == CUP_PRESENT)
            {
                state = State::await_patrick_close;
            }
            break;
        case State::await_patrick_close:
            if(patrick_house_state == PATRICK_HOUSE_CLOSED)
            {
                sleep(WAIT_TIME_AFTER_PATRICK_CLOSE); // ahh so stupdi
                state = State::drive_out_of_patrick;
            }
            break;
        case State::drive_out_of_patrick:
            if(bot_ir_state == IR_AT_END)
            {
                create_msg(comms::MsgId::drive_forward, bot_msg);
                bot_serial.write(bot_msg);
            } else if(bot_ir_state == IR_IN_MIDDLE)
            {
                sleep(PATRICK_HYSTERIPUSSY_TIME_SECONDS);
                state = State::drive_to_squidward;                
            }
            break;
        case State::drive_to_squidward:
            if(squidward_cup_state == CUP_NOT_PRESENT)
            {
                create_msg(comms::MsgId::drive_forward, bot_msg);
                bot_serial.write(bot_msg);
            } else if (squidward_cup_state == CUP_PRESENT)
            {
                sleep(SQUIDWARD_HYSTERIPUSSY_TIME_SECONDS);
                create_msg(comms::MsgId::drive_stop, bot_msg);
                bot_serial.write(bot_msg);
                state = State::fill_cup;
            }
            break;
        case State::fill_cup:
            create_msg(comms::MsgId::open_spigot, structure_msg);
            structure_serial.write(structure_msg);
            memcpy(structure_msg, 0, sizeof(structure_msg)); // wipe message
            sleep(CUP_FILL_TIME_SECONDS);
            create_msg(comms::MsgId::close_spigot, structure_msg);
            structure_serial.write(structure_msg);
            state = State::drive_to_sponge_end;
            break;
        case State::drive_to_sponge_end:
            if(bot_ir_state = IR_IN_MIDDLE)
            {
                create_msg(comms::MsgId::drive_forward, bot_msg);
                bot_serial.write(bot_msg);
            }
            if(bot_ir_state = IR_AT_END && spongebob_cup_state == CUP_PRESENT)
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
                state = State::drive_out_of_spongebob;
            }
            break;
        case State::drive_out_of_spongebob:
            if(bot_ir_state == IR_AT_END)
            {
                create_msg(comms::MsgId::drive_backward, bot_msg);
                bot_serial.write(bot_msg);
            } else if(bot_ir_state == IR_IN_MIDDLE)
            {
                sleep(SPONGEBOB_HYSTERIPUSSY_TIME_SECONDS);
                state = State::drive_to_patrick_start;                
            }
            break;            
        }
        
    RCLCPP_INFO(this->get_logger(), "IR_STATE: %d", bot_ir_state);
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
                request_random_sound();
                char msg[comms::MAX_MSG_SIZE_BYTES] = "";
                create_msg(comms::MsgId::comm_ack, msg);
                bot_serial.write(msg);
            }
            
            if(buffer.find(static_cast<uint8_t>(comms::MsgId::ir_state)) != std::string::npos){
                if(buffer.find("(A)") != std::string::npos)
                {
                    bot_ir_state = 1;
                    return;
                }
                if(buffer.find("(B)") != std::string::npos)
                {
                    bot_ir_state = 0;
                    return;
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
                request_random_sound();
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

void EWeekApp::request_random_sound()
{
    if (sound_files.empty()) return;
    
    // Static generator to seed once
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sound_files.size() - 1);
    
    std::string random_file = sound_files[dis(gen)];
    request_sound(random_file);
}