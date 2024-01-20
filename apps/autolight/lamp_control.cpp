#include "lamp_control.h"

#include <cassert>
#include <cmath>
#include <stdexcept>

namespace
{
template <typename T>
bool matchesTime(const T& timing, unsigned int minute_of_day);
}

void handleLamp(std::time_t time, float ambientlight,
                const std::optional<std::time_t>& last_motion_detector_time,
                const LampSettings& settings, LampInfo& status, std::mt19937& ran_gen,
                std::function<void(bool)> switch_function)
{
   const bool ison_by_ambientlight = shouldBeOnByAmbientLight(
      ambientlight, status.state, settings.ambient_light_threshold,
      settings.ambient_light_hysteresis);
   const bool ison_by_time = shouldBeOnByTime(time, settings.timings, status, ran_gen);
   const bool ison_by_motion =
      shouldBeOnByMotionDetector(time, settings.motion, last_motion_detector_time);
   const bool nominal_state = ison_by_ambientlight && (ison_by_time || ison_by_motion);
   if (status.state == LampState::UNKNOWN || nominal_state != toBool(status.state))
   {
      switch_function(nominal_state);
      status.state = fromBool(nominal_state);
   }
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

bool shouldBeOnByTime(std::time_t time, const std::vector<LampTime>& timings,
                      LampInfo& status, std::mt19937& ran_gen)
{
   struct tm tm;
   localtime_r(&time, &tm);
   const unsigned int minute_of_day = tm.tm_hour * 60 + tm.tm_min;

   for (const auto& timing : timings)
   {
      const bool matches_weekday =
         static_cast<bool>(timing.weekday & getWeekday(tm.tm_wday));
      if (matches_weekday && timing.random.has_value()
          && status.day_for_random != static_cast<unsigned int>(tm.tm_yday))
      {
         status.random_times = initialiseRandomTimes(timing, ran_gen);
         status.day_for_random = static_cast<unsigned int>(tm.tm_yday);
      }

      if (matches_weekday && matchesTime(timing, minute_of_day))
      {
         if (!timing.random.has_value())
         {
            return true;
         }
         for (const auto& random_timing : status.random_times)
         {
            if (matchesTime(random_timing, minute_of_day))
            {
               return true;
            }
         }
      }
   }
   return false;
}

bool shouldBeOnByMotionDetector(std::time_t time,
                                const std::optional<MotionDetectorSettings>& settings,
                                const std::optional<std::time_t>& motion_trigger_time)
{
   if (!settings.has_value() || !motion_trigger_time.has_value())
   {
      return false;
   }
   const std::time_t end_time = motion_trigger_time.value() + settings->on_time * 60ul;
   return time < end_time;
}

std::vector<OnOffTime> initialiseRandomTimes(const LampTime& timing,
                                             std::mt19937& ran_gen)
{
   assert(timing.random.has_value());
   assert(timing.random->count > 0);

   std::vector<unsigned int> lengths(timing.random->count);
   std::normal_distribution<float> length_dist(timing.random->average_length,
                                               timing.random->length_stddev);
   unsigned int sum_length = 0;
   for (auto& length : lengths)
   {
      length = static_cast<unsigned int>(std::round(length_dist(ran_gen)));
      sum_length += length;
   }

   const unsigned int nominal_on_length = timing.off - timing.on;
   const unsigned int off_time =
      nominal_on_length > sum_length ? nominal_on_length - sum_length : nominal_on_length;
   const unsigned int max_length = off_time / std::max(1u, timing.random->count - 1u);
   std::uniform_int_distribution<unsigned int> wait_dist(2u, std::max(15u, max_length));

   std::vector<OnOffTime> result(timing.random->count);
   unsigned int start = timing.on;
   for (unsigned int index = 0; index < timing.random->count; ++index)
   {
      auto& item = result[index];
      item.on = start + wait_dist(ran_gen);
      item.off = item.on + lengths[index];
      start = item.off;
   }

   return result;
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

namespace
{
template <typename T>
bool matchesTime(const T& timing, unsigned int minute_of_day)
{
   return minute_of_day >= timing.on && minute_of_day <= timing.off;
}
}  // namespace
