#include "settings_reader.h"

#include <libconfig.h++>

#include <locale>
#include <sstream>
#include <stdexcept>

Settings readSettings(const std::string& path)
{
   try
   {
      libconfig::Config conf;
      conf.readFile(path.c_str());
      conf.setAutoConvert(true);  // automatically convert from int to double

      Settings settings;
      settings.mqtt_host = static_cast<std::string>(conf.lookup("MQTT.host"));
      settings.mqtt_topic_base = static_cast<std::string>(conf.lookup("MQTT.topic_base"));
      settings.mqtt_topic_suffix =
         static_cast<std::string>(conf.lookup("MQTT.topic_suffix"));

      settings.gpio_chip = static_cast<std::string>(conf.lookup("GPIO_chip"));

      const auto& conf_gpios = conf.lookup("GPIOs");
      if (conf_gpios.getType() != libconfig::Setting::TypeList)
      {
         throw std::runtime_error("Error near line "
                                  + std::to_string(conf_gpios.getSourceLine())
                                  + ": 'GPIOs' must be a list");
      }
      for (const auto& conf_gpio : conf_gpios)
      {
         GpioSetting gpio_setting;
         gpio_setting.name = static_cast<std::string>(conf_gpio.lookup("name"));
         gpio_setting.number = static_cast<unsigned int>(conf_gpio.lookup("number"));
         settings.gpios.push_back(std::move(gpio_setting));
      }

      return settings;
   }
   catch (libconfig::ParseException& ex)
   {
      throw std::runtime_error("Error parsing configuration file '" + path + "' line "
                               + std::to_string(ex.getLine()) + ": " + ex.getError());
   }
   catch (libconfig::FileIOException& ex)
   {
      throw std::runtime_error("Configuration file '" + path + "' not found.");
   }
   catch (libconfig::SettingNotFoundException& ex)
   {
      throw std::runtime_error("Setting '" + std::string(ex.getPath()) + "' not found");
   }
   catch (libconfig::SettingTypeException& ex)
   {
      throw std::runtime_error("Type error in setting '" + std::string(ex.getPath())
                               + "'");
   }
}
