#pragma once

#include "lamp_settings.h"
#include <string>
#include <unordered_map>

struct Settings
{
   std::string mqtt_host;
   std::string ambient_light_topic;
   std::unordered_map<std::string, std::string> motion_detectors;  // topic -> name
   std::vector<LampSettings> lamps;
};
