#pragma once
#include "rtcstuff.h"

enum class State : uint8_t
{
    date = 0,
    time,
    temp,
    set_time,
    _num
};


struct Message
{
    enum class Type : uint8_t
    {
        none,
        button_press,
    } type;
};

void states_init();

void state_act();

void check_and_transition(Message message);
