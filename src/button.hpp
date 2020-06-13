#pragma once
#include <inttypes.h>

static constexpr uint8_t nextButton = 0;
static constexpr uint8_t upButton = 1;

void button_init();

bool getButton(uint8_t id);

bool getButtonDebounce(uint8_t id);
