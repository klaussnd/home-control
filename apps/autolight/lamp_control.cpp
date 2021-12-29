#include "lamp_control.h"

#include <stdexcept>

void handleLamp(std::time_t time, float ambientlight, const LampSettings& settings,
                LampState& previous_state, std::function<void(bool)> switch_function)
{
   const bool nominal_state =
      shouldLampBeOn(time, ambientlight, previous_state, settings);
   if (previous_state == LampState::UNKNOWN || nominal_state != toBool(previous_state))
   {
      switch_function(nominal_state);
      previous_state = fromBool(nominal_state);
   }
}

bool shouldLampBeOn(std::time_t time, float ambientlight, LampState previous_state,
                    const LampSettings& lamp_settings)
{
   const bool ison_by_ambientlight = shouldBeOnByAmbientLight(
      ambientlight, previous_state, lamp_settings.ambient_light_threshold,
      lamp_settings.ambient_light_hysteresis);
   const bool ison_by_time = shouldBeOnByTime(time, lamp_settings.timings);
   return ison_by_ambientlight && ison_by_time;
}

bool shouldBeOnByAmbientLight(float ambientlight, LampState previous_state,
                              float ambient_light_threshold,
                              float ambient_light_hysteresis)
{
   if (previous_state == LampState::ON)
   {
      ambient_light_threshold += ambient_light_hysteresis;
   }
   else if (previous_state == LampState::OFF)
   {
      ambient_light_threshold -= ambient_light_hysteresis;
   }
   return ambientlight <= ambient_light_threshold;
}

bool shouldBeOnByTime(std::time_t time, const std::vector<LampTime>& timings)
{
   struct tm tm;
   localtime_r(&time, &tm);
   const unsigned int minute_of_day = tm.tm_hour * 60 + tm.tm_min;

   for (const auto& timing : timings)
   {
      const bool matches_weekday =
         static_cast<bool>(timing.weekday & getWeekday(tm.tm_wday));
      if (matches_weekday && minute_of_day >= timing.on && minute_of_day <= timing.off)
      {
         return true;
      }
   }
   return false;
}

Weekday getWeekday(int wday)
{
   switch (wday)
   {
   case 0:
      return Weekday::SUNDAY;
   case 1:
      return Weekday::MONDAY;
   case 2:
      return Weekday::TUESDAY;
   case 3:
      return Weekday::WEDNESDAY;
   case 4:
      return Weekday::THURSDAY;
   case 5:
      return Weekday::FRIDAY;
   case 6:
      return Weekday::SATURDAY;
   }
   throw std::logic_error("Invalid day of the week " + std::to_string(wday));
}

bool toBool(LampState lamp_state)
{
   switch (lamp_state)
   {
   case LampState::ON:
      return true;
   case LampState::OFF:
      return false;
   case LampState::UNKNOWN:
      throw std::logic_error("Unknown lampstate cannot be converted to bool");
   }
   throw std::logic_error("Undefined LampState "
                          + std::to_string(static_cast<int>(lamp_state)));
}

LampState fromBool(bool state)
{
   return state ? LampState::ON : LampState::OFF;
}
