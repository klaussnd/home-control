#pragma once

#include <string>
#include <vector>

struct GpioSetting
{
   std::string name;
   unsigned int number{0};
};

struct Settings
{
   std::string mqtt_host;
   std::string mqtt_topic_base;
   std::string gpio_chip;
   std::vector<GpioSetting> gpios;
};
