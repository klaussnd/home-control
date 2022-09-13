#include "lamp_control.h"
#include "mqtt.h"
#include "settings_reader.h"
#include <mqttbase/MqttClient.h>
#include <mqttbase/helper.h>

#include <cerrno>
#include <csignal>
#include <cstring>
#include <ctime>
#include <iostream>
#include <optional>
#include <thread>

volatile bool run = true;
volatile bool reread_config = false;

void siginthandler(int signo);
void sighuphandler(int signo);

int main(void)
{
   if (::signal(SIGINT, siginthandler) == SIG_ERR)
   {
      std::cerr << "Unable to register signal handler: " << strerror(errno) << std::endl;
   }
   if (::signal(SIGHUP, sighuphandler) == SIG_ERR)
   {
      std::cerr << "Unable to register signal handler: " << strerror(errno) << std::endl;
   }

   try
   {
      const std::string settingsFile = homeDirectory() + "/.config/autolight.cfg";
      auto settings = readSettings(settingsFile);
      MqttClient mqtt{
         MqttClientSettings{settings.mqtt_host, {}, {}, "lightcontroller", ""}};
      AmbientLightMqttCallback ambient_light(mqtt, settings.ambient_light_topic);
      mqtt.setCallback(ambient_light);
      mqtt.connectWait();
      std::cout << "Connected to MQTT server " << settings.mqtt_host << std::endl;

      std::vector<LampState> lamp_state(settings.lamps.size(), LampState::UNKNOWN);

      while (run)
      {
         if (ambient_light.ambientLight().has_value())
         {
            const std::time_t time = std::time(nullptr);
            for (std::size_t index = 0; index < settings.lamps.size(); ++index)
            {
               const auto& lamp_settings = settings.lamps[index];
               auto& old_state = lamp_state[index];
               handleLamp(time, ambient_light.ambientLight().value(), lamp_settings,
                          old_state, [&mqtt, &lamp_settings](bool ison) {
                             for (const auto& topic : lamp_settings.topic)
                             {
                                switchLamp(mqtt, topic, ison);
                             }
                             std::cout << "Switching " << lamp_settings.name << ' '
                                       << (ison ? "on" : "off") << std::endl;
                          });
            }
         }

         if (reread_config)
         {
            reread_config = false;
            try
            {
               std::cout << "Re-reading configuration ... " << std::flush;
               settings = readSettings(settingsFile);
               std::cout << "done" << std::endl;
            }
            catch (const std::exception& err)
            {
               std::cout << "failed: " << err.what() << std::endl;
            }
         }
         std::this_thread::sleep_for(std::chrono::seconds(1));
      }
   }
   catch (const std::exception& err)
   {
      std::cerr << "Error: " << err.what() << std::endl;
      return EXIT_FAILURE;
   }

   std::cout << "Exit." << std::endl;
   return EXIT_SUCCESS;
}

void siginthandler(int)
{
   if (run == false)
      exit(EXIT_FAILURE);
   else
      run = false;
}

void sighuphandler(int)
{
   reread_config = true;
}
