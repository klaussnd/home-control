#include "settings_reader.h"

#include <libconfig.h++>

#include <locale>
#include <sstream>
#include <stdexcept>

#include <iostream>

namespace
{
ValueConfig parseValueConfigSettings(const libconfig::Setting& conf_lamp);
}

Settings readSettings(const std::string& path)
{
   try
   {
      libconfig::Config conf;
      conf.readFile(path.c_str());
      conf.setAutoConvert(true);  // automatically convert from int to double

      Settings settings;
      settings.mqtt_host = static_cast<std::string>(conf.lookup("MQTT.host"));

      const auto& conf_items = conf.lookup("items");
      if (conf_items.getType() != libconfig::Setting::TypeList)
      {
         throw std::runtime_error("Error near line "
                                  + std::to_string(conf_items.getSourceLine())
                                  + ": 'items' must be a list");
      }
      for (const auto& conf_item : conf_items)
      {
         settings.value_config.push_back(parseValueConfigSettings(conf_item));
      }

      return settings;
   }
   catch (libconfig::ParseException& ex)
   {
      throw std::runtime_error("Error parsing configuration file '" + path + "' line "
                               + std::to_string(ex.getLine()) + ": " + ex.getError());
   }
   catch (libconfig::FileIOException&)
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

namespace
{
std::vector<Action> parseActions(const libconfig::Setting& conf_actions);

ValueConfig parseValueConfigSettings(const libconfig::Setting& conf_item)
{
   ValueConfig item;
   item.name = static_cast<std::string>(conf_item.lookup("name"));
   item.topic = static_cast<std::string>(conf_item.lookup("topic"));
   item.json_ptr = static_cast<std::string>(conf_item.lookup("json_ptr"));
   item.min = static_cast<float>(conf_item.lookup("min"));
   item.max = static_cast<float>(conf_item.lookup("max"));
   item.action_min = parseActions(conf_item.lookup("action_min"));
   item.action_max = parseActions(conf_item.lookup("action_max"));
   return item;
}

std::vector<Action> parseActions(const libconfig::Setting& conf_actions)
{
   if (conf_actions.getType() != libconfig::Setting::TypeList)
   {
      throw std::runtime_error("Error near line "
                               + std::to_string(conf_actions.getSourceLine())
                               + ": actions must be a list");
   }

   std::vector<Action> actions;
   for (const auto& conf_action : conf_actions)
   {
      actions.push_back(Action{static_cast<std::string>(conf_action.lookup("topic")),
                               static_cast<std::string>(conf_action.lookup("payload"))});
   }
   return actions;
}
}  // namespace
