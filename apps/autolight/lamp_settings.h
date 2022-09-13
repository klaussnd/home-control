#pragma once

#include "bitmask.h"

#include <cstddef>
#include <string>
#include <vector>

enum class Weekday
{
   MONDAY = 1 << 0,
   TUESDAY = 1 << 1,
   WEDNESDAY = 1 << 2,
   THURSDAY = 1 << 3,
   FRIDAY = 1 << 4,
   SATURDAY = 1 << 5,
   SUNDAY = 1 << 6,
};

ENABLE_BITMASK(Weekday);

struct LampTime
{
   Weekday weekday;
   // times in minutes since the beginning of the day
   unsigned int on;
   unsigned int off;
};

struct LampSettings
{
   std::string name;
   std::vector<std::string> topic;
   float ambient_light_threshold;
   float ambient_light_hysteresis;
   std::vector<LampTime> timings;
};
