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
uint8_t repeat = 0;
uint8_t num_repetitions = 0;

struct Alert
{
    uint8_t dayOfWeek;       // 0 = Sunday, 1 = Monday, ... 6 = Saturday, 255 = every day
    uint8_t hour;
    uint8_t minute;
    const char* message;
};

static constexpr uint8_t ALERT_LENGTH_SEC = 30;
Alert alerts[] = {
    {255, 11, 30, "ESSEN FASSEN! MAMPF MAMPF"},
    {  5, 16, 30, "FEIERABEND! BLOSS WEG HIER!"},
    {255, 00, 00, "WURSTKOPF ARSCHBERGER FICKO"},
};

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
    for(unsigned digit = 0; digit < 8; digit++)
    {
        /*
        Serial.print(t[digit]);
        Serial.print(" ");
        Serial.print(~digit & 0b10 ? 1 : 0);
        Serial.print(~digit & 0b01 ? 1 : 0);
        Serial.print(" ");
        Serial.print(!(digit & 0b100) ? 1 : 0);
        Serial.print((digit & 0b100) ? 1 : 0);
        Serial.print(" ");
        */
        digitalWrite(DIGIT[1], ~digit & 0b01);
        digitalWrite(DIGIT[0], ~digit & 0b10);
        for(unsigned i = 0; i < sizeof(DATA); i++)
        {
            digitalWrite(DATA[i], digit < length ? (1 << i) & t[digit] : 0);
            //Serial.print( (1 << i) & t[digit] ? 1 : 0);
        }
        //Serial.print(" ");
        digitalWrite(WRITE[0], !(digit & 0b100));
        digitalWrite(WRITE[1], digit & 0b100);
        //while(!Serial.available() ){};Serial.read();
        digitalWrite(WRITE[0], 1);
        digitalWrite(WRITE[1], 1);
        //delay(2000);
        //Serial.println();
    }
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
    Serial.println("C");

    for(Alert& alert : alerts)
    {
        if(alert.dayOfWeek == 255 || now.DayOfWeek() == alert.dayOfWeek)
        {
            if(alert.hour == now.Hour() && alert.minute == now.Minute() && now.Second() < ALERT_LENGTH_SEC)
            {
                auto_show(alert.message, strlen(alert.message));
                return;
            }
        }
    }

    static char buf[9];
    switch(state)
    {
        case State::time:
            if(repeat == 0)
                num_repetitions = 20;
            snprintf_P(buf,
                    sizeof(buf),
                    PSTR("%02u:%02u:%02u"),
                    now.Hour(),
                    now.Minute(),
                    now.Second()
            );
            if(repeat % 2)
            {
                buf[2] = ' ';
                buf[5] = ' ';
            }
            break;
        case State::date:
            if(repeat == 0)
                num_repetitions = 2;
            snprintf_P(buf,
                    sizeof(buf),
                    PSTR("%02u.%02u.%02u"),
                    now.Day(),
                    now.Month(),
                    now.Year()
            );
            break;
        case State::temp:
            if(repeat == 0)
                num_repetitions = 1;
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
    //Serial.println(buf);
    auto_show(buf, 8);
    delay(1000);
    if(++repeat > num_repetitions)
    {
        state = static_cast<State>((static_cast<unsigned>(state) + 1) % static_cast<unsigned>(State::_num));
        repeat = 0;
    }
}
