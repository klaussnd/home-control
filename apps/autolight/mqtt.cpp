#include "mqtt.h"

#include <mqttbase/helper.h>

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
      std::cout << timeToString(std::time(nullptr)) << " Received ambient light "
                << m_light << std::endl;
   }
}

MotionDetectorMqttCallback::MotionDetectorMqttCallback(
   MqttClient& mqtt_client, const std::unordered_map<std::string, std::string>& topics)
      : m_client(mqtt_client)
      , m_topics(topics)
{
}

MotionDetectorMqttCallback::~MotionDetectorMqttCallback() = default;

std::optional<std::time_t> MotionDetectorMqttCallback::lastDetectionTime(
   const std::string& detector_name)
{
   const auto it = m_motion_times.find(detector_name);
   if (it == m_motion_times.end())
   {
      return {};
   }
   return it->second;
}

void MotionDetectorMqttCallback::connected()
{
   for (const auto& [topic, _] : m_topics)
   {
      m_client.subscribe(topic);
   }
}

void MotionDetectorMqttCallback::messageArrived(const std::string& topic,
                                                const std::string& /*value*/)
{
   const auto it = m_topics.find(topic);
   if (it != m_topics.end())
   {
      const auto& name = it->second;
      const std::time_t current_time = std::time(nullptr);
      m_motion_times[name] = current_time;
      std::cout << timeToString(current_time) << " Motion detector " << name << std::endl;
   }
}

void switchLamp(MqttClient& mqtt_client, const std::string& topic, bool on)
{
   mqtt_client.publish(topic, on ? "ON" : "OFF");
}
