#include <Arduino.h>
#include "states.hpp"
#include "display.hpp"
#include "config.hpp"

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
                    now.Year()
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

        default:
            Serial.println("there is something wrong with the state");
            break;
    }

    auto_show(buf, 8);
};

void check_and_transition(Message message)
{
    frame++;
    State next_state = state;

    switch(message.type)
    {
        case Message::Type::button_press:
            next_state = State::set_time;
            break;
        default:    //Normal operation
            switch(state)
            {
                case State::date:
                    if(frame == 2)
                        next_state = State::time;
                    break;
                case State::time:
                    if(frame == 20)
                        next_state = State::temp;
                    break;
                case State::temp:
                    if(frame == 2)
                        next_state = State::date;
                    break;
                case State::set_time:
                    //no return yet
                    break;
                default:
                    next_state = State::time;
            }
    }

    if(next_state != state)
    {
        state = next_state;
        frame = 0;
    }
};
