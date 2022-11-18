#pragma once

#include "../lamp_settings.h"

#include <ctime>

LampSettings makeSettings();
std::time_t makeTime(Weekday weekday, int hour, int minute);
