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

void siginthandler(int signo);

int main(void)
{
   if (::signal(SIGINT, siginthandler) == SIG_ERR)
   {
      std::cerr << "Unable to register signal handler: " << strerror(errno) << std::endl;
   }

   try
   {
      const std::string settingsFile = homeDirectory() + "/.config/autolight.cfg";
      const auto settings = readSettings(settingsFile);
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
                             switchLamp(mqtt, lamp_settings.topic, ison);
                             std::cout << "Switching " << lamp_settings.name << ' '
                                       << (ison ? "on" : "off") << std::endl;
                          });
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
