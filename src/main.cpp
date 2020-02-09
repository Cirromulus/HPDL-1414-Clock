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
    state = State::time;
}

void print(const char* t, unsigned length)
{
    //delay(500);
    for(unsigned digit = 0; digit < length; digit++)
    {
        //Serial.print(t[digit]);
        //Serial.print(" ");
        digitalWrite(DIGIT[0], ~digit & 0b10);
        //Serial.print(~digit & 0b10 ? 1 : 0);
        digitalWrite(DIGIT[1], ~digit & 0b01);
        //Serial.print(~digit & 0b01 ? 1 : 0);
        digitalWrite(WRITE[0], !(digit & 0b100));
        //Serial.print(!(digit & 0b100) ? 1 : 0);
        digitalWrite(WRITE[1], digit & 0b100);
        //Serial.print((digit & 0b100) ? 1 : 0);
        //Serial.print(" - ");

        for(unsigned i = 0; i < sizeof(DATA); i++)
        {
            digitalWrite(DATA[i], (1 << i) & t[digit]);
            Serial.print( (1 << i) & t[digit] ? 1 : 0);
        }
        digitalWrite(WRITE[0], 1);
        digitalWrite(WRITE[1], 1);
        //Serial.println();
    }
    //delay(500);
}

void scroll(const char* string, unsigned length)
{
    for(unsigned i = 0; i < length; i++)
    {
        print(&string[i], i + 8 <= length ? 8 : length - i);
        delay(200);
    }
}

void auto_show(const char* string, unsigned length)
{
    if(length <= 8)
        print(string, length);
    else
        scroll(string, length);
}

char buf[9];
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

    switch(state)
    {
        case State::time:
            Serial.println("Time mode");
            snprintf_P(buf,
                    sizeof(buf),
                    PSTR("%02u:%02u:%02u"),
                    now.Hour(),
                    now.Minute(),
                    now.Second()
            );
            break;
        case State::date:
            Serial.println("Date mode");
            snprintf_P(buf,
                    sizeof(buf),
                    PSTR("%02u.%02u.%02u"),
                    now.Day(),
                    now.Month(),
                    now.Year()
            );
            break;
        case State::temp:
            Serial.println("Temp mode");
            snprintf_P(buf,
                    sizeof(buf),
                    PSTR(" %04d C "),
                    int(temp.AsFloatDegC()*100)
            );
            buf[4] = buf[3];
            buf[3] = '.';
            break;
        default:
            state = State::time;
            break;
    }
    Serial.println(buf);
    auto_show(buf, 8);
    delay(1000);
    state = static_cast<State>((static_cast<unsigned>(state) + 1) % static_cast<unsigned>(State::_num));
}
