#include "config.hpp"

//bool button_state[2];


void button_init()
{
    for(auto button : BUTTONS)
    {
        pinMode(button, INPUT);
        pinMode(button, INPUT_PULLUP);
    }
}


bool getButton(uint8_t id)
{
    if(id > 1)
        return false;
    return !digitalRead(BUTTONS[id]);
}

bool getButtonDebounce(uint8_t id)
{
    if(getButton(id))
    {
        while(getButton(id))
            delay(10);
        return true;
    }
    else
    {
        return false;
    }
}
