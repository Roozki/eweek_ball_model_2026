#pragma once
// Arduino based comms
#include <Arduino.h>
#include "drive.h"
#include "main_datatypes.h"
#include "comm_protocol.h"

namespace comms {

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
            // Serial.println("Stop");
            break;
        case MsgId::drive_forward:
            drive.setState(DriveState::forward);
            // Serial.println("Forward");
            break;
        case MsgId::drive_backward: 
            drive.setState(DriveState::backward);
            // Serial.println("Backwards");
            break;
        case MsgId::comm_ack:
            if(machine.get_state() == State::await_comms_ack)
            {
                if(machine.signal_comms_acked())
                {
                    Serial.println("Comms acked!");
                }
            }
        case MsgId::open_spigot:
            
            spigot.open();
            Serial.println("Spigot: Open");

            break;
        case MsgId::close_spigot:
            Serial.println("Spigot: Close");

            spigot.close();
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