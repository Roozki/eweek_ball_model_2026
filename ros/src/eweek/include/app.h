#include <rclcpp/rclcpp.hpp>
// #include "sensor_msgs/msg/joy.hpp"
#include <thread>
#include <chrono>

#include <serial/serial.h>
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/string.hpp>

#include "../../../../platformio/eweek_2026/include/comm_protocol.h"

#define TX_UART_BUFF_SIZE_BYTES 128
#define RX_UART_BUFF_SIZE_BYTES 128

#define BOT_RX_POLL_RATE 50
#define STRUCTURE_RX_POLL_RATE 50


using std::string;
using namespace comms;

class EWeekApp : public rclcpp::Node {
public:
    EWeekApp();

    // float firm


       //   angle_echo.positions.resize(NUM_JOINTS);
    // void start_rx() {
    //     serialRxThread = std::thread(&ArmSerial::serial_rx(), this);
    // }
    // string joint_names[6] = {"joint_turntable", "joint_axis1", "joint_axis2", "joint_axis3", "joint_axis4", "joint_ender"}; //? old arm urdf


private:

    unsigned long esp_bot_baud = 115200;//19200;
    string esp_bot_port = "/dev/serial/by-id/usb-1a86_USB_Serial-if00-port0";

    serial::Serial bot_esp;
    serial::Timeout timeout_uart = serial::Timeout::simpleTimeout(1000); // E.g., 1000 ms or 1 second


    // rclcpp::Subscription<rover_msgs::msg::ArmCommand>::SharedPtr command_subscriber;

    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_subscriber;

    void CmdVelCallback(const geometry_msgs::msg::Twist::SharedPtr msg);

    // rclcpp

    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr speaker_publisher;

    // sensor_msgs::msg::JointState prev_joint_states; //for sim
    // rclcpp::Publisher<rover_msgs::msg::ArmCommand>::SharedPtr arm_position_publisher;

    rclcpp::TimerBase::SharedPtr bot_rx_timer_;
    rclcpp::TimerBase::SharedPtr structure_rx_timer_;

    std::thread serialRxThread;

    void serial_rx_bot_esp(); //? should be static inline?
    void serial_rx_structure(); //? should be static inline?
    void create_msg(MsgId msg_id, char (&msg)[MAX_MSG_SIZE_BYTES], char *data = nullptr);
    void request_sound(std::string filename);


  };
  



void EWeekApp::create_msg(MsgId msg_id, char (&msg)[MAX_MSG_SIZE_BYTES], char *data)
{
    // char msg[MAX_MSG_SIZE_BYTES] = "";
    strncat(msg, MSG_STARTER, sizeof(MSG_STARTER));
    char msg_id_char = '0';
    memcpy(&msg_id_char, &msg_id, sizeof(msg_id_char));
    strncat(msg, &msg_id_char, MSG_ID_SIZE_BYTES);
    strncat(msg, DATA_OPENER, sizeof(DATA_OPENER));
    if(data != nullptr)
    {
        strncat(msg, data, MAX_DATA_SIZE_BYTES);
    }
    strncat(msg, DATA_CLOSER, sizeof(DATA_CLOSER));

}