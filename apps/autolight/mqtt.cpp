#include "mqtt.h"

#include <iostream>

AmbientLightMqttCallback::AmbientLightMqttCallback(MqttClient& mqtt_client,
                                                   const std::string& topic)
      : m_client(mqtt_client)
      , m_topic(topic)
      , m_light(0.0f)
      , m_has_value(false)
{
}

AmbientLightMqttCallback::~AmbientLightMqttCallback() = default;

std::optional<float> AmbientLightMqttCallback::ambientLight() const
{
   if (m_has_value)
   {
      return m_light;
   }
   return {};
}

void AmbientLightMqttCallback::connected()
{
   m_client.subscribe(m_topic);
}

void AmbientLightMqttCallback::messageArrived(const std::string& topic,
                                              const std::string& value)
{
   if (topic == m_topic)
   {
      m_light = std::stof(value);
      m_has_value = true;
      std::cout << "Received ambient light " << m_light << std::endl;
   }
}

void switchLamp(MqttClient& mqtt_client, const std::string& topic, bool on)
{
   mqtt_client.publish(topic, on ? "ON" : "OFF");
}
