#include "button.hpp"
#include "display.hpp"
#include "rtcstuff.h"

enum class State : uint8_t
{
    day = 0,
    month,
    year,
    hour,
    minute,
    save    //important to be at the end
};

void set_time()
{
    State state = static_cast<State>(0);
    char buf[9];
    uint8_t frame = 0;
    RtcDateTime new_time = now;

    bool up_button_pressed = false;
    uint8_t cycles_up_button_pressed = 0;


    while(true)
    {
        //action
        switch(state)
        {
            case State::hour:
                snprintf_P(buf,
                        sizeof(buf),
                        PSTR("%02u:%02u:%02u"),
                        new_time.Hour(),
                        new_time.Minute(),
                        new_time.Second()
                );
                if(up_button_pressed)
                {
                    new_time = RtcDateTime(
                        new_time.Year(),
                        new_time.Month(),
                        new_time.Day(),
                        (new_time.Hour() + 1) % 24,
                        new_time.Minute(),
                        new_time.Second()
                    );
                }
                if(frame % 2)
                {
                    buf[0] = ' ';
                    buf[1] = ' ';
                }
                break;
            case State::minute:
                snprintf_P(buf,
                        sizeof(buf),
                        PSTR("%02u:%02u:%02u"),
                        new_time.Hour(),
                        new_time.Minute(),
                        new_time.Second()
                );
                if(up_button_pressed)
                {
                    new_time = RtcDateTime(
                        new_time.Year(),
                        new_time.Month(),
                        new_time.Day(),
                        new_time.Hour(),
                        (new_time.Minute() + 1) % 60,
                        new_time.Second()
                    );
                }
                if(frame % 2)
                {
                    buf[3] = ' ';
                    buf[4] = ' ';
                }
                break;
            case State::day:
                snprintf_P(buf,
                        sizeof(buf),
                        PSTR("%02u.%02u.%02u"),
                        new_time.Day(),
                        new_time.Month(),
                        new_time.Year()%100
                );
                if(up_button_pressed)
                {
                    new_time = RtcDateTime(
                        new_time.Year(),
                        new_time.Month(),
                        (new_time.Day() + 1) % 31,
                        new_time.Hour(),
                        new_time.Minute(),
                        new_time.Second()
                    );
                }
                if(frame % 2)
                {
                    buf[0] = ' ';
                    buf[1] = ' ';
                }
                break;
            case State::month:
                snprintf_P(buf,
                        sizeof(buf),
                        PSTR("%02u.%02u.%02u"),
                        new_time.Day(),
                        new_time.Month(),
                        new_time.Year()%100
                );
                if(up_button_pressed)
                {
                    new_time = RtcDateTime(
                        new_time.Year(),
                        (new_time.Month() + 1) % 12,
                        new_time.Day(),
                        new_time.Hour(),
                        new_time.Minute(),
                        new_time.Second()
                    );
                }
                if(frame % 2)
                {
                    buf[3] = ' ';
                    buf[4] = ' ';
                }
                break;
            case State::year:
                snprintf_P(buf,
                        sizeof(buf),
                        PSTR("%02u.%02u.%02u"),
                        new_time.Day(),
                        new_time.Month(),
                        new_time.Year()%100
                );
                if(up_button_pressed)
                {
                    new_time = RtcDateTime(
                        2000 + ((new_time.Year() + 1) % 100),
                        new_time.Month(),
                        new_time.Day(),
                        new_time.Hour(),
                        new_time.Minute(),
                        new_time.Second()
                    );
                }
                if(frame % 2)
                {
                    buf[6] = ' ';
                    buf[7] = ' ';
                }
                break;
            case State::save:
                memcpy(buf, PSTR("  SAVE? "), 9);
                break;
        }

        auto_show(buf, 8);

        //transition
        if(getButtonDebounce(nextButton))
        {
            switch(state)
            {
                case State::save:
                    now = new_time;
                    Rtc.SetDateTime(new_time);
                    return; //END
                default:
                    state = static_cast<State>(static_cast<uint8_t>(state) + 1);
            }
        }

        frame++;
        bool was_button_pressed = up_button_pressed;
        up_button_pressed = false;

        for(uint8_t i = 0; i < (cycles_up_button_pressed > 3 ? 10 : 50) ; i++)
        {   //poor mans buttoncheck
            up_button_pressed |= getButton(upButton);
            delay(10);
        }

        //small delay for less frequent "oops, one too many"
        if(cycles_up_button_pressed == 0 && up_button_pressed)
        {
            //first time
            delay(100);
        }

        if(was_button_pressed == true && up_button_pressed == true)
        {
            cycles_up_button_pressed ++;
        }
        else
        {
            cycles_up_button_pressed = 0;
        }
    }

}
