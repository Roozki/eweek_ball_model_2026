#pragma once   
// #include <cstdint>

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
    sound_played    = 0xA1, // A for acks?
    comm_ack        = 0xA2,

    // Raw drive commands
    drive_stop      = 0xD0,
    drive_forward   = 0xD1,
    drive_backward   = 0xD2,

    // "Position" drive commands
    drive_to_squidward,     // detect cup.
    drive_to_spongebob_end, // IR sensor detection
    drive_to_patrick_start, // IR sensor detection
    // mcu -> computer

    play_sound = 0xC1, //?
    comm_ping = 0xE1,
 
    squidward_cup = 0xC2,
    open_spigot = 0xC3,
    close_spigot = 0xC4,

    patrick_house_state, // 1 = open, 0 = closed

    bot_location

};

}