#pragma once

#include <mqttbase/MqttClient.h>

#include <atomic>
#include <optional>

class AmbientLightMqttCallback : public MqttCallback
{
public:
   AmbientLightMqttCallback(MqttClient& mqtt_client, const std::string& topic);
   ~AmbientLightMqttCallback();

   std::optional<float> ambientLight() const;

private:
   void connected() override;
   void messageArrived(const std::string& topic, const std::string& value) override;

private:
   MqttClient& m_client;
   std::string m_topic;
   std::atomic<float> m_light;
   std::atomic<bool> m_has_value;
};

void switchLamp(MqttClient& mqtt_client, const std::string& topic, bool on);
