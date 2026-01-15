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

    bot_rx_timer_ = this->create_wall_timer(
    std::chrono::duration<double>(bot_period), std::bind(&EWeekApp::bot_rx, this));

    structure_rx_timer_ = this->create_wall_timer(
        std::chrono::duration<double>(structure_period), std::bind(&EWeekApp::structure_rx, this));

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
        RCLCPP_WARN(this->get_logger(), "%s", buffer.c_str());
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
        RCLCPP_WARN(this->get_logger(), "%s", buffer.c_str());
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
                return;
            }
            int msgid_pos = buffer.find(static_cast<uint8_t>(comms::MsgId::squidward_cup));
            if(msgid_pos != std::string::npos)
            {
                static bool flag = false;
                if(buffer.find("(1)") != std::string::npos)
                {
                    // Squidward cup detected
                    if(!flag)
                    {
                        request_sound("bleep.wav");
                    }
                    flag = true;
                }

                if(buffer.find("(0)") != std::string::npos)
                {
                    flag = false;
                }
                // else is a parsing error

            }

            msgid_pos = buffer.find(static_cast<uint8_t>(comms::MsgId::patrick_house_state))
            if(msgid_pos != std::string::npos)
            {
                
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