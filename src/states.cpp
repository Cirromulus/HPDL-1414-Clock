#include <Arduino.h>
#include "states.hpp"
#include "display.hpp"
#include "config.hpp"
#include "button.hpp"
#include "set_time.hpp"

static State state;
static uint8_t frame;

void states_init()
{
    state = State::time;
    frame = 0;
}

void state_act()
{
    static char buf[9];
    printDateTime(now);
    Serial.println();
    temp.Print(Serial);
    Serial.println("C");

    switch(state)
    {
        case State::time:
            snprintf_P(buf,
                    sizeof(buf),
                    PSTR("%02u:%02u:%02u"),
                    now.Hour(),
                    now.Minute(),
                    now.Second()
            );
            if(now.Second() % 2)
            {
                buf[2] = ' ';
                buf[5] = ' ';
            }
            break;
        case State::date:
            snprintf_P(buf,
                    sizeof(buf),
                    PSTR("%02u.%02u.%02u"),
                    now.Day(),
                    now.Month(),
                    now.Year()%100
            );
            break;
        case State::temp:
            snprintf_P(buf,
                    sizeof(buf),
                    PSTR(" %04d C "),
                    int(temp.AsFloatDegC()*100)
            );
            buf[4] = buf[3];
            buf[3] = '.';
            break;
        case State::set_time:
            set_time();
            memcpy(buf, PSTR("SAVE OK "), 9);
            break;
        default:
            Serial.println("there is something wrong with the state");
            break;
    }

    auto_show(buf, 8);
};

void check_and_transition()
{
    frame++;
    State next_state = state;

    switch(state)
    {
        case State::date:
            if(frame == 2 || getButtonDebounce(upButton))
                next_state = State::time;
            break;
        case State::time:
            if(frame == 20 || getButtonDebounce(upButton))
                next_state = State::temp;
            break;
        case State::temp:
            if(frame == 2 || getButtonDebounce(upButton))
                next_state = State::date;
            break;
        case State::set_time:
            next_state = State::time;
            break;
        default:
            next_state = State::time;
    }

    //button override
    if(getButtonDebounce(nextButton))
    {
        next_state = State::set_time;
    }

    if(next_state != state)
    {
        state = next_state;
        frame = 0;
    }
};
