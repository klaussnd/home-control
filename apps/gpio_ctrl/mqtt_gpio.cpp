#include "mqtt_gpio.h"

#include <algorithm>

#include <iostream>

GpioMqttCallback::GpioMqttCallback(MqttClient& mqtt_client, gpiod::chip& chip,
                                   const std::vector<GpioSetting>& settings)
      : m_client(mqtt_client)
{
   const std::string app_name{"gpio_ctrl"};
   for (const auto& gpio : settings)
   {
      auto& channel =
         m_channels.emplace_back(LineData{chip.get_line(gpio.number), gpio.name});
      channel.gpio_line.request(
         gpiod::line_request{app_name, gpiod::line_request::DIRECTION_OUTPUT, 0});
   }
}

GpioMqttCallback::~GpioMqttCallback()
{
   for (auto& item : m_channels)
   {
      item.gpio_line.release();
   }
}

void GpioMqttCallback::connected()
{
   m_client.subscribe("#");
}

void GpioMqttCallback::messageArrived(const std::string& topic, const std::string& value)
{
   auto it =
      std::find_if(m_channels.begin(), m_channels.end(), [&topic](const auto& channel) {
         const std::string expected_topic = channel.name + '/' + "cmnd/POWER";
         return expected_topic == topic;
      });
   if (it != m_channels.end())
   {
      const bool ison = value == "ON";
      const bool isoff = value == "OFF";
      if (ison || isoff)
      {
         it->gpio_line.set_value(ison);
         std::cout << "Switching " << it->name << " " << (ison ? "on" : "off")
                   << std::endl;
      }
   }
}
