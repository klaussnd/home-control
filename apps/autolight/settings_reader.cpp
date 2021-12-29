#include "settings_reader.h"
#include "lamp_control.h"

#include <libconfig.h++>

#include <locale>
#include <sstream>
#include <stdexcept>

namespace
{
Weekday parseWeekdays(const libconfig::Setting& conf);
unsigned int parseTime(const libconfig::Setting& conf);
}  // namespace

Settings readSettings(const std::string& path)
{
   try
   {
      libconfig::Config conf;
      conf.readFile(path.c_str());
      conf.setAutoConvert(true);  // automatically convert from int to double

      Settings settings;
      settings.mqtt_host = static_cast<std::string>(conf.lookup("MQTT.host"));
      settings.ambient_light_topic =
         static_cast<std::string>(conf.lookup("ambientlight.topic"));
      const float hysteresis = static_cast<float>(conf.lookup("ambientlight.hysteresis"));

      const auto& conf_lamps = conf.lookup("lamps");
      if (conf_lamps.getType() != libconfig::Setting::TypeList)
      {
         throw std::runtime_error("'lamps' must be a list");
      }
      for (const auto& conf_lamp : conf_lamps)
      {
         LampSettings lamp_settings;
         lamp_settings.name = static_cast<std::string>(conf_lamp.lookup("name"));
         lamp_settings.topic = static_cast<std::string>(conf_lamp.lookup("mqtt_topic"));
         lamp_settings.ambient_light_threshold =
            static_cast<float>(conf_lamp.lookup("ambientlight_threshold"));
         lamp_settings.ambient_light_hysteresis = hysteresis;

         const auto& conf_schedule = conf_lamp.lookup("schedule");
         if (conf_schedule.getType() != libconfig::Setting::TypeList)
         {
            throw std::runtime_error("'schedule' must be a list");
         }
         for (const auto& schedule_item : conf_schedule)
         {
            LampTime timing;
            timing.weekday = parseWeekdays(schedule_item.lookup("weekdays"));
            timing.on = parseTime(schedule_item.lookup("on"));
            timing.off = parseTime(schedule_item.lookup("off"));
            lamp_settings.timings.push_back(std::move(timing));
         }

         settings.lamps.push_back(std::move(lamp_settings));
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

namespace
{
Weekday parseWeekday(const std::string& s);

Weekday parseWeekdays(const libconfig::Setting& conf)
{
   if (conf.getType() != libconfig::Setting::TypeList)
   {
      throw std::runtime_error("Error near line " + std::to_string(conf.getSourceLine())
                               + ": 'weekdays' must be a list");
   }
   Weekday weekdays = static_cast<Weekday>(0);
   for (const auto& item : conf)
   {
      const Weekday wday = parseWeekday(item);
      weekdays |= wday;
   }
   return weekdays;
}
unsigned int parseTime(const libconfig::Setting& conf)
{
   if (conf.getType() == libconfig::Setting::TypeList && conf.getLength() == 2)
   {
      return static_cast<unsigned int>(conf[0]) * 60u
             + static_cast<unsigned int>(conf[1]);
   }
   throw std::runtime_error(
      "Error near line " + std::to_string(conf.getSourceLine())
      + ": time must be a 2-element list containing minutes and seconds");
}

Weekday parseWeekday(const std::string& wday)
{
   std::istringstream ss(wday);
   std::ios_base::iostate err = std::ios_base::goodbit;
   struct tm tm;
   std::use_facet<std::time_get<char>>(ss.getloc()).get_weekday({ss}, {}, ss, err, &tm);
   ss.setstate(err);
   if (ss)
   {
      return getWeekday(tm.tm_wday);
   }

   throw std::runtime_error("Cannot parse weekday " + wday);
}
}  // namespace
