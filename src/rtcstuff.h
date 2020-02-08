#pragma once
#include <RtcDS3231.h>
#include <Wire.h>

extern RtcDS3231<TwoWire> Rtc;

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt);

void rtcSetup();
