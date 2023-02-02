#pragma once

#include "settings.h"

#include <mqttbase/MqttClient.h>

#include <gpiod.hpp>

class GpioMqttCallback : public MqttCallback
{
public:
   GpioMqttCallback(MqttClient& mqtt_client, gpiod::chip& chip,
                    const std::string& topic_suffix,
                    const std::vector<GpioSetting>& settings);
   ~GpioMqttCallback();

private:
   void connected() override;
   void messageArrived(const std::string& topic, const std::string& value) override;

private:
   struct LineData
   {
      gpiod::line gpio_line;
      std::string name;
   };

   MqttClient& m_client;
   std::string m_topic_suffix;
   std::vector<LineData> m_channels;
};
