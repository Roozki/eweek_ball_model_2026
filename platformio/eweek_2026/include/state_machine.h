#pragma once


enum class State {
    startup,
    await_comms_ack, 
    await_cup_open
    
};

class StateMachine {
public:
    StateMachine() {};

    void init()
    {
        current_state = State::await_comms_ack;
    }
    State get_state()
    {
        return this->current_state;
    }

    uint32_t last_comm_ack_msg_timestamp_ms = 0;
    uint32_t comm_ack_msg_period_ms = 500;

    // Signal functions - signal to the state machine that something happened
    bool signal_comms_acked()
    {
        if(current_state == State::await_comms_ack)
        {
            current_state = State::await_cup_open;
            return true;
        }
        return false;
    }

private:
    State current_state = State::startup;
};