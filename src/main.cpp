#include <Arduino.h>
#include "rtcstuff.h"

static constexpr uint8_t DATA[] =
{
    2,3,4,5,6,7,8
};

static constexpr uint8_t DIGIT[] =
{
    9,10
};

static constexpr uint8_t WRITE[] =
{
    11,12
};


enum class State : unsigned
{
    date = 0,
    time,
    temp,
    _num
} state;


void setup() {
    for(auto pin : DATA)
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, 0);
    }
    for(auto pin : DIGIT)
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, 0);
    }
    for(auto pin : WRITE)
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, 1);
    }
    Serial.begin(115200);

    rtcSetup();
    state = State::temp;
}

void print(char* t, unsigned length)
{
    for(unsigned digit = 0; digit < length; digit++)
    {
        digitalWrite(DIGIT[0], ~digit & 0b10);
        digitalWrite(DIGIT[1], ~digit & 0b01);
        delayMicroseconds(1);
        digitalWrite(WRITE[0], !(digit & 0b100));
        digitalWrite(WRITE[1], digit & 0b100);
        for(unsigned i = 0; i < sizeof(DATA); i++)
        {
            digitalWrite(DATA[i], (1 << i) & t[digit]);
        }
        digitalWrite(WRITE[0], 1);
        digitalWrite(WRITE[1], 1);
        delayMicroseconds(1);
    }
}

void scroll(char* string, unsigned length)
{
    for(unsigned i = 0; i < length; i++)
    {
        print(&string[i], i + 8 <= length ? 8 : length - i);
        delay(200);
    }
}

void auto_show(char* string, unsigned length)
{
    if(length <= 8)
        print(string, length);
    else
        scroll(string, length);
}


void loop() {
    if (!Rtc.IsDateTimeValid())
    {
        if (Rtc.LastError() != 0)
        {
            // we have a communications error
            // see https://www.arduino.cc/en/Reference/WireEndTransmission for
            // what the number means
            Serial.print("RTC communications error = ");
            Serial.println(Rtc.LastError());
        }
        else
        {
            // Common Causes:
            //    1) the battery on the device is low or even missing and the power line was disconnected
            Serial.println("RTC lost confidence in the DateTime!");
        }
    }
    RtcDateTime now = Rtc.GetDateTime();
    printDateTime(now);
    Serial.println();
    RtcTemperature temp = Rtc.GetTemperature();
    temp.Print(Serial);
    // you may also get the temperature as a float and print it
    Serial.println("C");

    static char buf[9];
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
            auto_show(buf, 8);
            state = State::date;
            break;
        case State::date:
            snprintf_P(buf,
                    sizeof(buf),
                    PSTR("%02u.%02u.%02u"),
                    now.Day(),
                    now.Month(),
                    now.Year()
            );
            auto_show(buf, 8);
            state = State::temp;
            break;
        case State::temp:
            state = State::time;
        break;
            snprintf_P(buf,
                    sizeof(buf),
                    PSTR("%.2f C "),
                    temp.AsFloatDegC()
            );
            auto_show(buf, 8);
            break;
        default:
            state = State::time;
            break;
    }
    delay(1000);
}
