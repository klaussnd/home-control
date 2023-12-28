#include "helper.h"

#include <cstring>
#include <stdexcept>

namespace
{
int getWeekday(Weekday weekday);
}

LampSettings makeSettings()
{
   LampSettings settings;
   settings.name = "test lamp";
   settings.topic = {"test/topic"};
   settings.ambient_light_threshold = 900.0f;
   settings.ambient_light_hysteresis = 5.0f;
   settings.timings = {{Weekday::SUNDAY, 8 * 60, 10 * 60, {}}};
   return settings;
}

std::time_t makeTime(Weekday weekday, int hour, int minute)
{
   struct tm tm;
   std::memset(&tm, 0, sizeof(tm));
   tm.tm_hour = hour;
   tm.tm_min = minute;
   tm.tm_mday = 19 + getWeekday(weekday);
   tm.tm_mon = 11 - 1;
   tm.tm_year = 2017 - 1900;
   return std::mktime(&tm);
}

int getYearDay(std::time_t time)
{
   struct tm tm;
   localtime_r(&time, &tm);
   return tm.tm_yday;
}

namespace
{
int getWeekday(Weekday weekday)
{
   switch (weekday)
   {
   case Weekday::SUNDAY:
      return 0;
   case Weekday::MONDAY:
      return 1;
   case Weekday::TUESDAY:
      return 2;
   case Weekday::WEDNESDAY:
      return 3;
   case Weekday::THURSDAY:
      return 4;
   case Weekday::FRIDAY:
      return 5;
   case Weekday::SATURDAY:
      return 6;
   }

   throw std::runtime_error("Unknow weekday");
}
}  // namespace
