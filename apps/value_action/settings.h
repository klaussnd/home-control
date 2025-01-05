#pragma once

#include "linearcolourmap.h"

#include <optional>
#include <string>
#include <vector>

struct Action
{
   std::string topic;
   std::string payload;
};

struct Visualisation
{
   std::string topic;
   std::string payload;  ///< Should contain %r, %g, %b which are replaced by red, green,
                         ///< and blue value, respectively
   ColourMapType colour_map_type{ColourMapType::GREEN_RED};
   unsigned int brightness_percent{100u};  ///< brightness of the colour scale in percent
};

struct ValueConfig
{
   std::string name;      ///< Name for display purpose only
   std::string topic;     ///< MQTT topic where the value is broadcasted
   std::string json_ptr;  ///< JSON pointer where to find the value in the payload, or
                          ///< empty if the payload is directly the value
   float min{0.f};
   std::vector<Action> action_min;
   float max{0.f};
   std::vector<Action> action_max;

   std::optional<Visualisation> visualisation;
};

struct Settings
{
   std::string mqtt_host;
   std::vector<ValueConfig> value_config;
};
