#pragma once

#include "../lamp_settings.h"

#include <ctime>

LampSettings makeSettings();
std::time_t makeTime(Weekday weekday, int hour, int minute);
int getYearDay(std::time_t time);
