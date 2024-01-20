#pragma once

#include <mqttbase/MqttClient.h>

#include <atomic>
#include <ctime>
#include <optional>
#include <unordered_map>

class AmbientLightMqttCallback : public MqttCallback
{
public:
   AmbientLightMqttCallback(MqttClient& mqtt_client, const std::string& topic);
   ~AmbientLightMqttCallback() override;

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

class MotionDetectorMqttCallback : public MqttCallback
{
public:
   MotionDetectorMqttCallback(MqttClient& mqtt_client,
                              const std::unordered_map<std::string, std::string>& topics);
   ~MotionDetectorMqttCallback() override;

   std::optional<std::time_t> lastDetectionTime(const std::string& detector_name);

private:
   void connected() override;
   void messageArrived(const std::string& topic, const std::string& value) override;

private:
   MqttClient& m_client;
   std::unordered_map<std::string, std::string> m_topics;        // topic -> name
   std::unordered_map<std::string, std::time_t> m_motion_times;  // name -> time triggered
};

void switchLamp(MqttClient& mqtt_client, const std::string& topic, bool on);
