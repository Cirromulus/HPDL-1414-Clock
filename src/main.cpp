#include <Arduino.h>

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
}

void paint(char* t, unsigned length)
{
    for(unsigned digit = 0; digit < length; digit++)
    {
        digitalWrite(DIGIT[0], ~digit & 0b10);
        digitalWrite(DIGIT[1], ~digit & 0b01);
        delay(1);
        digitalWrite(WRITE[0], !(digit & 0b100));
        digitalWrite(WRITE[1], digit & 0b100);
        for(unsigned i = 0; i < sizeof(DATA); i++)
        {
            digitalWrite(DATA[i], (1 << i) & t[digit]);
        }
        digitalWrite(WRITE[0], 1);
        digitalWrite(WRITE[1], 1);
        delay(1);
    }
}

void loop() {

    char c[] = "ARSCHWAND FICKBERGER WURSTKOPF BUMSMANN";

    for(unsigned i = 0; i < sizeof(c); i++)
    {
        paint(&c[i], i + 8 <= sizeof(c) ? 8 : sizeof(c) - i);
        delay(300);
    }

}
