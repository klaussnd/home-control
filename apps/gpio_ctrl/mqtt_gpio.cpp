#include "mqtt_gpio.h"

#include <algorithm>

#include <iostream>

GpioMqttCallback::GpioMqttCallback(MqttClient& mqtt_client, gpiod::chip& chip,
                                   const std::string& topic_suffix,
                                   const std::vector<GpioSetting>& settings)
      : m_client(mqtt_client)
{
   if (!topic_suffix.empty())
   {
      m_topic_suffix = '/' + topic_suffix;
   }
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
   std::vector<std::string> topics;
   for (const auto& channel : m_channels)
   {
      topics.push_back(channel.name + m_topic_suffix);
   }
   m_client.subscribe(topics);
}

void GpioMqttCallback::messageArrived(const std::string& topic, const std::string& value)
{
   auto it = std::find_if(m_channels.begin(), m_channels.end(),
                          [&topic, &suffix = m_topic_suffix](const auto& channel)
                          {
                             const std::string expected_topic = channel.name + suffix;
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
