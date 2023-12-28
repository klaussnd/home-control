#pragma once

#include "lamp_settings.h"

#include <ctime>
#include <functional>
#include <limits>
#include <optional>
#include <random>

enum class LampState
{
   ON,
   OFF,
   UNKNOWN,
};

// times in minutes since the beginning of the day
struct OnOffTime
{
   unsigned int on{0};
   unsigned int off{0};
};

struct LampInfo
{
   LampState state{LampState::UNKNOWN};
   unsigned int day_for_random{
      std::numeric_limits<unsigned int>::max()};  // day of the year for the random times
   std::vector<OnOffTime> random_times;
};

/** Handle one lamp
 *
 *  When a lamp switches its state (from on to off or vice versa), @e switch_function
 *  is called. The @e status is updated.
 */
void handleLamp(std::time_t time, float ambientlight, const LampSettings& settings,
                LampInfo& status, std::mt19937& ran_gen,
                std::function<void(bool)> switch_function);
bool shouldBeOnByAmbientLight(float ambientlight, LampState previous_state,
                              float ambient_light_threshold,
                              float ambient_light_hysteresis);
bool shouldBeOnByTime(std::time_t time, const std::vector<LampTime>& timings,
                      LampInfo& status, std::mt19937& ran_gen);

std::vector<OnOffTime> initialiseRandomTimes(const LampTime& timing,
                                             std::mt19937& ran_gen);

/// Convert weekday as returned in struct tm
Weekday getWeekday(int wday);

bool toBool(LampState lamp_state);
LampState fromBool(bool state);
