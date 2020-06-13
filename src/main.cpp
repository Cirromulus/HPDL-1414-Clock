#include <Arduino.h>
#include "config.hpp"
#include "display.hpp"
#include "rtcstuff.h"
#include "states.hpp"


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

    for(auto button : BUTTONS)
    {
        pinMode(button, INPUT);
        pinMode(button, INPUT_PULLUP);
    }
    Serial.begin(115200);

    rtcSetup();
    display_init();
    states_init();
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

    Serial.print("Buttons: ");
    for(auto butt : BUTTONS)
    {
        Serial.print(digitalRead(butt) ? "1" : "x");
    }
    Serial.println();

    rtcUpdate();

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


    state_act();
    check_and_transition(Message{Message::Type::none});
    delay(999);        //not really necessary to being precise
}
