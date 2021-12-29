#pragma once

#include "lamp_settings.h"
#include <string>

struct Settings
{
   std::string mqtt_host;
   std::string ambient_light_topic;
   std::vector<LampSettings> lamps;
};
