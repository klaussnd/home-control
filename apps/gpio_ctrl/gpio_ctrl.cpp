#include "mqtt_gpio.h"
#include "settings_reader.h"

#include <mqttbase/MqttClient.h>
#include <mqttbase/helper.h>

#include <gpiod.hpp>

#include <cerrno>
#include <csignal>
#include <cstring>
#include <iostream>
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
      const std::string settingsFile = homeDirectory() + "/.config/gpio-ctrl.cfg";
      const auto settings = readSettings(settingsFile);
      std::cout << "Read settings: " << settings.gpios.size() << " GPIOs" << std::endl;
      gpiod::chip gpio_chip{settings.gpio_chip};

      MqttClient mqtt{MqttClientSettings{
         settings.mqtt_host, {}, {}, "gpio-ctrl", settings.mqtt_topic_base}};
      GpioMqttCallback callback(mqtt, gpio_chip, settings.gpios);
      mqtt.setCallback(callback);
      mqtt.connectWait();
      std::cout << "Connected to MQTT server " << settings.mqtt_host << std::endl;

      while (run)
      {
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
