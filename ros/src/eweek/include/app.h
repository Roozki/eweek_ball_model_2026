#include <rclcpp/rclcpp.hpp>
// #include "sensor_msgs/msg/joy.hpp"
#include <thread>
#include <chrono>
#include <vector>
#include <random>

#include <serial/serial.h>
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/string.hpp>

#include "../../../../platformio/eweek_2026/include/comm_protocol.h"

#define TX_UART_BUFF_SIZE_BYTES 128
#define RX_UART_BUFF_SIZE_BYTES 128

#define BOT_RX_POLL_RATE 50
#define STRUCTURE_RX_POLL_RATE 50

#define PATRICK_HOUSE_CLOSED 0
#define PATRICK_HOUSE_OPEN 1

#define CUP_NOT_PRESENT 0
#define CUP_PRESENT 1

#define IR_AT_END 1
#define IR_IN_MIDDLE 0

inline static constexpr float CUP_FILL_TIME_SECONDS = 5.0;
inline static constexpr float WAIT_TIME_AFTER_PATRICK_CLOSE = 1.0;
inline static constexpr float PATRICK_HYSTERIPUSSY_TIME_SECONDS = 0.1;
inline static constexpr float SQUIDWARD_HYSTERIPUSSY_TIME_SECONDS = 0.05;
inline static constexpr float SPONGEBOB_HYSTERIPUSSY_TIME_SECONDS = 0.05;
inline static constexpr float USER_CUP_TAKE_HYSTERISUSSY_TIME_SECONDS = 5.0;


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

    unsigned long bot_serial_baud = 115200;
    unsigned long structure_serial_baud = 115200;
    string bot_serial_port = "/dev/serial/by-id/usb-1a86_USB_Serial-if00-port0";
    string structure_serial_port = "/dev/serial/by-id/usb-1a86_USB2.0-Serial-if00-port0";

    serial::Serial bot_serial;
    serial::Serial structure_serial;
    
    serial::Timeout bot_serial_timeout = serial::Timeout::simpleTimeout(100);
    serial::Timeout arduino_serial_timeout = serial::Timeout::simpleTimeout(100);


    // rclcpp::Subscription<rover_msgs::msg::ArmCommand>::SharedPtr command_subscriber;

    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_subscriber;

    void CmdVelCallback(const geometry_msgs::msg::Twist::SharedPtr msg);

    // rclcpp

    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr speaker_publisher;

    // sensor_msgs::msg::JointState prev_joint_states; //for sim
    // rclcpp::Publisher<rover_msgs::msg::ArmCommand>::SharedPtr arm_position_publisher;

    rclcpp::TimerBase::SharedPtr bot_rx_timer_;
    rclcpp::TimerBase::SharedPtr structure_rx_timer_;
    rclcpp::TimerBase::SharedPtr app_timer;

    // std::thread bot_?serialRxThread;

    void bot_rx(); //? should be static inline?
    void structure_rx(); //? should be static inline?
    void create_msg(MsgId msg_id, char (&msg)[MAX_MSG_SIZE_BYTES], char *data = nullptr);
    void request_sound(std::string filename);
    void run_app();

    int bot_ir_state = -1;
    int patrick_house_state = -1; 

    int patrick_cup_state = -1;
    int squidward_cup_state = -1;
    int spongebob_cup_state = -1;

    enum class State {
        startup,
        drive_out_of_patrick,
        drive_to_patrick_start,
        drive_to_squidward,
        drive_to_sponge_end,
        drive_out_of_spongebob,
        await_patrick_open,
        await_patrick_cup,
        await_patrick_close,
        fill_cup, // open, wait, close
        await_user_take_cup,

    };

    State state = State::startup;

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