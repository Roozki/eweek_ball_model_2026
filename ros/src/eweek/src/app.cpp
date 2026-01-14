#include <app.h>

using namespace comms;

EWeekApp::EWeekApp() : Node("EWeek_App")
{
    auto qos = rclcpp::QoS(rclcpp::KeepLast(1)).reliable().durability_volatile(); //? AHHHH WHAT THE FUCK IS A QOS

        cmd_vel_subscriber = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel", 10, std::bind(&EWeekApp::CmdVelCallback, this, std::placeholders::_1));
    speaker_publisher = this->create_publisher<std_msgs::msg::String>(
        "/speaker/command", 10);

    bot_esp.setPort(esp_bot_port);
    bot_esp.setBaudrate(esp_bot_baud);
    bot_esp.open();
    double period = 1.0 / BOT_RX_POLL_RATE;

    bot_rx_timer_ = this->create_wall_timer(
    std::chrono::duration<double>(period),std::bind(&EWeekApp::serial_rx_bot_esp, this));
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
    if(msg->linear.x > 1)
    {
        create_msg(comms::MsgId::drive_forward, serial_msg);
    } else if(msg->linear.x < -1)
    {
        create_msg(comms::MsgId::drive_backward, serial_msg);
    } else {
        create_msg(comms::MsgId::drive_stop, serial_msg);
    }
    bot_esp.write(serial_msg);
    return;
}


void EWeekApp::serial_rx_bot_esp()
{
    std::string next_char = "";
    std::string buffer = "";
    int timeoutCounter = 0;
   if (bot_esp.available() > 0){
       // ROS_WARN("Reading");

        //timeoutCounter ++;
       // next_char = teensy.read(); 
        buffer = bot_esp.read(RX_UART_BUFF_SIZE_BYTES);
        RCLCPP_WARN(this->get_logger(), "%s", buffer.c_str());
        // if(next_char == "\n" || next_char == "\r" || next_char == "\0"){
        //     timeoutCounter = RX_UART_BUFF;
        // }
        if (buffer.size() > 0){
            if(buffer.find(static_cast<uint8_t>(comms::MsgId::comm_ping)) != std::string::npos){
                RCLCPP_INFO(this->get_logger(), "comms acked - esp");
                request_sound("bleep.wav");
                char msg[comms::MAX_MSG_SIZE_BYTES] = "";
                create_msg(comms::MsgId::comm_ack, msg);
                bot_esp.write(msg);

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