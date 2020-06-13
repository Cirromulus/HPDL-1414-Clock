#include "display.hpp"


void display_init()
{
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
