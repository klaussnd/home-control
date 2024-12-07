#include "mqtt_gpio.h"

#include <algorithm>

#include <iostream>

namespace
{
gpiod::line_request makeGpioRequest(gpiod::chip& chip,
                                    const std::vector<GpioSetting>& settings);
}

GpioMqttCallback::GpioMqttCallback(MqttClient& mqtt_client, gpiod::chip& chip,
                                   const std::string& topic_suffix,
                                   const std::vector<GpioSetting>& settings)
      : m_client(mqtt_client)
      , m_gpio_request(makeGpioRequest(chip, settings))
      , m_channels(settings)
{
   if (!topic_suffix.empty())
   {
      m_topic_suffix = '/' + topic_suffix;
   }
}

GpioMqttCallback::~GpioMqttCallback()
{
   m_gpio_request.release();
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
         m_gpio_request.set_value(
            it->number, ison ? gpiod::line::value::ACTIVE : gpiod::line::value::INACTIVE);
         std::cout << "Switching " << it->name << " " << (ison ? "on" : "off")
                   << std::endl;
      }
   }
}

namespace
{
gpiod::line_request makeGpioRequest(gpiod::chip& chip,
                                    const std::vector<GpioSetting>& settings)
{
   constexpr auto app_name = "gpio_ctrl";
   auto request_builder = chip.prepare_request();
   request_builder.set_consumer(app_name);
   for (const auto& gpio : settings)
   {
      request_builder.add_line_settings(gpio.number, gpiod::line_settings().set_direction(
                                                        gpiod::line::direction::OUTPUT));
   }
   return request_builder.do_request();
}
}  // namespace
