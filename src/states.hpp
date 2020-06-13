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

void states_init();

void state_act();

void check_and_transition();
