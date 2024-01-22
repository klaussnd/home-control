#include "settings_reader.h"
#include "lamp_control.h"

#include <libconfig.h++>

#include <locale>
#include <sstream>
#include <stdexcept>

#include <iostream>

namespace
{
std::unordered_map<std::string, std::string> parseMotionDetectors(
   const libconfig::Setting& conf);
LampSettings parseLampSettings(const libconfig::Setting& conf_lamp, float hysteresis);
LampTime parseLampTiming(const libconfig::Setting& conf_schedule_item);
Weekday parseWeekdays(const libconfig::Setting& conf);
unsigned int parseTime(const libconfig::Setting& conf);

void checkConsistency(const Settings& settings);
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
      if (conf.exists("motion_detectors"))
      {
         settings.motion_detectors =
            parseMotionDetectors(conf.lookup("motion_detectors"));
      }

      const auto& conf_lamps = conf.lookup("lamps");
      if (conf_lamps.getType() != libconfig::Setting::TypeList)
      {
         throw std::runtime_error("Error near line "
                                  + std::to_string(conf_lamps.getSourceLine())
                                  + ": 'lamps' must be a list");
      }
      for (const auto& conf_lamp : conf_lamps)
      {
         settings.lamps.push_back(parseLampSettings(conf_lamp, hysteresis));
      }

      checkConsistency(settings);

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
Weekday parseWeekday(const std::string& s);

std::unordered_map<std::string, std::string> parseMotionDetectors(
   const libconfig::Setting& conf)
{
   if (conf.getType() != libconfig::Setting::TypeList)
   {
      throw std::runtime_error("Error near line " + std::to_string(conf.getSourceLine())
                               + ": 'motion_detectors' must be a list");
   }

   std::unordered_map<std::string, std::string> motion_detectors;
   for (const auto& item : conf)
   {
      std::string name = static_cast<std::string>(item.lookup("name"));
      std::string topic = static_cast<std::string>(item.lookup("topic"));
      motion_detectors.insert(std::make_pair(std::move(topic), std::move(name)));
   }

   return motion_detectors;
}

LampSettings parseLampSettings(const libconfig::Setting& conf_lamp, float hysteresis)
{
   LampSettings lamp_settings;
   lamp_settings.name = static_cast<std::string>(conf_lamp.lookup("name"));
   const auto& conf_topic = conf_lamp.lookup("mqtt_topic");
   if (conf_topic.getType() != libconfig::Setting::TypeList)
   {
      throw std::runtime_error("Error near line "
                               + std::to_string(conf_topic.getSourceLine())
                               + ": 'mqtt_topic' must be a list");
   }
   for (const auto& topic : conf_topic)
   {
      lamp_settings.topic.push_back(static_cast<std::string>(topic));
   }
   lamp_settings.ambient_light_threshold =
      static_cast<float>(conf_lamp.lookup("ambientlight_threshold"));
   lamp_settings.ambient_light_hysteresis = hysteresis;

   const auto& conf_schedule = conf_lamp.lookup("schedule");
   if (conf_schedule.getType() != libconfig::Setting::TypeList)
   {
      throw std::runtime_error("Error near line "
                               + std::to_string(conf_schedule.getSourceLine())
                               + ": 'schedule' must be a list");
   }
   for (const auto& conf_schedule_item : conf_schedule)
   {
      lamp_settings.timings.push_back(std::move(parseLampTiming(conf_schedule_item)));
   }

   if (conf_lamp.exists("motion"))
   {
      const auto& conf_motion = conf_lamp.lookup("motion");
      if (conf_motion.getType() != libconfig::Setting::TypeGroup)
      {
         throw std::runtime_error("Error near line "
                                  + std::to_string(conf_motion.getSourceLine())
                                  + ": 'motion' must be a group");
      }
      MotionDetectorSettings motion;
      motion.detector_name = static_cast<std::string>(conf_motion.lookup("detector"));
      motion.on_time = static_cast<unsigned int>(conf_motion.lookup("on_time"));
      lamp_settings.motion = std::move(motion);
   }

   return lamp_settings;
}

LampTime parseLampTiming(const libconfig::Setting& conf_schedule_item)
{
   LampTime timing;
   timing.weekday = parseWeekdays(conf_schedule_item.lookup("weekdays"));
   timing.on = parseTime(conf_schedule_item.lookup("on"));
   timing.off = parseTime(conf_schedule_item.lookup("off"));

   if (conf_schedule_item.exists("random"))
   {
      const auto& conf_random = conf_schedule_item.lookup("random");
      if (conf_random.getType() != libconfig::Setting::TypeGroup)
      {
         throw std::runtime_error("Error near line "
                                  + std::to_string(conf_random.getSourceLine())
                                  + ": 'random' must be a group");
      }
      RandomLampTime random;
      random.count = static_cast<unsigned int>(conf_random.lookup("count"));
      if (random.count == 0)
      {
         throw std::runtime_error("Error near line "
                                  + std::to_string(conf_random.getSourceLine())
                                  + ": random count must be > 0");
      }
      random.average_length =
         static_cast<unsigned int>(conf_random.lookup("average_length"));
      random.length_stddev =
         static_cast<unsigned int>(conf_random.lookup("length_stddev"));
      timing.random = std::move(random);
   }

   return timing;
}

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

void checkConsistency(const Settings& settings)
{
   std::unordered_map<std::string, unsigned int> motion_detector_use;
   for (const auto& [_, detector_name] : settings.motion_detectors)
   {
      motion_detector_use[detector_name] = 0u;
   }
   for (const auto& lamp : settings.lamps)
   {
      if (lamp.motion.has_value())
      {
         const auto& detector_name = lamp.motion.value().detector_name;
         const auto it = motion_detector_use.find(detector_name);
         if (it == motion_detector_use.end())
         {
            throw std::runtime_error(
               "Inconsistent settings: lamp " + lamp.name
               + " is configured to use non-existent motion detector " + detector_name);
         }
         it->second++;
      }
   }

   for (const auto& [detector_name, use_count] : motion_detector_use)
   {
      if (use_count == 0)
      {
         std::cout << "Note: Unused motion detector " + detector_name << '\n';
      }
   }
}
}  // namespace
