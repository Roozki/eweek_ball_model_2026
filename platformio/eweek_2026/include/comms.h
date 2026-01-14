#pragma once
// Arduino based comms
#include <Arduino.h>
#include <drive.h>
#include <main_datatypes.h>

namespace comms {
    // Startup message
    static inline constexpr char STARTUP_MSG[] = "PROGRAM_START";

// General params
static inline constexpr uint32_t MAX_DATA_SIZE_BYTES = 32u;
static inline constexpr uint32_t MAX_MSG_SIZE_BYTES = 48u;
static inline constexpr uint32_t MSG_ID_SIZE_BYTES = 1u;

static inline constexpr uint32_t MIN_RX_MSG_SIZE_BYTES = 3u;
static inline constexpr uint32_t MAX_RX_MSG_SIZE_BYTES = 8u;

// Msg structure: (TX + RX)
// Starter + Message ID + Data opener + data (optional) + data closer/message end

// Static parts
static inline constexpr uint32_t MSG_STARTER_OFFSET = 0u;
static inline constexpr uint32_t MESSAGE_ID_OFFSET = 1u;
static inline constexpr uint32_t MESSAGE_ID_SIZE = 1u;
static inline constexpr uint32_t DATA_OPENER_OFFSET = MESSAGE_ID_OFFSET + MESSAGE_ID_SIZE;
static inline constexpr uint32_t DATA_OFFSET = 3u;
// Then, message data is parsed by knowing the message id

static inline constexpr char MSG_STARTER[] = "$";
static inline constexpr char DATA_OPENER[] = "(";
static inline constexpr char DATA_CLOSER[] = ")\n"; // data closer + message end character (endline)

// Message IDs:
enum class MsgId : uint8_t {
    unspecified = 0x00,
    // mcu <- computer
    sound_played    = 0xA1, //?
    drive_stop      = 0xD0,
    drive_forward   = 0xD1,
    drive_backward   = 0xD2,
    // mcu -> computer
    play_sound = 0xC1, //?

};

// RX raw buffer (stores a single message)
    char raw_rx_buff[MAX_RX_MSG_SIZE_BYTES] = {};

void init()
{
    Serial.begin(115200);
    Serial.setTimeout(500);
}

void send(MsgId msg_id, char *data = nullptr)
{
    char msg[MAX_MSG_SIZE_BYTES] = "";
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

    // No need to use println, we already have the newline character
    Serial.print(msg);
}

void parse_buffer(char (&buff)[MAX_RX_MSG_SIZE_BYTES])
{
    // MSG Opener: $ - index 0
    uint8_t msg_id = 0;
    memcpy(&msg_id, buff, MESSAGE_ID_SIZE);
    Serial.println(msg_id);
    switch(static_cast<MsgId>(msg_id))
    {
        case MsgId::drive_stop:
            drive.setState(DriveState::stop);
            Serial.println("Stop");
            break;
        case MsgId::drive_forward:
            drive.setState(DriveState::forward);
            Serial.println("Forward");
            break;
        case MsgId::drive_backward: 
            drive.setState(DriveState::backward);
            Serial.println("Backwards");
            break;
        default:
            Serial.println("Unkown: Error");
            break;
    }

}


// Poll buffer, see if theres a message ready for us.
void poll_rx_buffer()
{
    if(Serial.available() >= MIN_RX_MSG_SIZE_BYTES)
    {
        Serial.println("Data in");
        memset(raw_rx_buff, 0, sizeof(raw_rx_buff));
        char garbage[MAX_MSG_SIZE_BYTES] = {};
        // Flush any reminants of last message
        Serial.readBytesUntil(MSG_STARTER[0], garbage, MAX_RX_MSG_SIZE_BYTES); 
        // Then grab actual message
        Serial.readBytesUntil('\n', raw_rx_buff, MAX_RX_MSG_SIZE_BYTES);
        Serial.println(raw_rx_buff);
        parse_buffer(raw_rx_buff);
        Serial.flush();
    } 
}

};