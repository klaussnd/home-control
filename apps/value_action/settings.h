#pragma once

#include <string>
#include <vector>

struct Action
{
   std::string topic;
   std::string payload;
};

struct ValueConfig
{
   std::string name;      ///< Name for display purpose only
   std::string topic;     ///< MQTT topic where the value is broadcasted
   std::string json_ptr;  ///< JSON pointer where to find the value in the payload, or
                          ///< empty if the payload is directly the value
   float min;
   std::vector<Action> action_min;
   float max;
   std::vector<Action> action_max;
};

struct Settings
{
   std::string mqtt_host;
   std::vector<ValueConfig> value_config;
};
