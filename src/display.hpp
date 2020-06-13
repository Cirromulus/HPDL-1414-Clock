#include <Arduino.h>
#include "config.hpp"

void display_init();

void print(const char* t, unsigned length);

void scroll(const char* string, unsigned length);

void auto_show(const char* string, unsigned length);
