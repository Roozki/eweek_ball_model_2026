// Arduino based comms
#include <Arduino.h>


namespace comms {
    // Startup message
    static inline constexpr char STARTUP_MSG[] = "PROGRAM_START";

// General params
static inline constexpr uint32_t MAX_DATA_SIZE_BYTES = 32u;
static inline constexpr uint32_t MAX_MSG_SIZE_BYTES = 48u;
static inline constexpr uint32_t MSG_ID_SIZE_BYTES = 1u;

// Msg structure:
// Starter + Message ID + Data opener + data (optional) + data closer/message end

static inline constexpr char MSG_STARTER[] = "$";
static inline constexpr char DATA_OPENER[] = "(";
static inline constexpr char DATA_CLOSER[] = ")\n"; // data closer + message end character (endline)

// Message IDs:
enum class MsgId : uint8_t {
    unspecified = 0x00,
    // mcu <- computer
    sound_played = 0xA1,
    
    // mcu -> computer
    play_sound = 0xC1,

};

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



};