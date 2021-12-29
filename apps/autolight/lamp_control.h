#pragma once

#include "lamp_settings.h"

#include <ctime>
#include <functional>
#include <optional>

enum class LampState
{
   ON,
   OFF,
   UNKNOWN,
};

void handleLamp(std::time_t time, float ambientlight, const LampSettings& settings,
                LampState& previous_state, std::function<void(bool)> switch_function);
bool shouldLampBeOn(std::time_t time, float ambientlight, LampState previous_state,
                    const LampSettings& lamp_settings);
bool shouldBeOnByAmbientLight(float ambientlight, LampState previous_state,
                              float ambient_light_threshold,
                              float ambient_light_hysteresis);
bool shouldBeOnByTime(std::time_t time, const std::vector<LampTime>& timings);

/// Convert weekday as returned in struct tm
Weekday getWeekday(int wday);

bool toBool(LampState lamp_state);
LampState fromBool(bool state);
