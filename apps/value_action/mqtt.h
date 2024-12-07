#pragma once

#include <mqttbase/MqttClient.h>

#include <functional>
#include <mutex>
#include <optional>
#include <string>

class ValueMqttCallback : public MqttCallback
{
public:
   struct MessageInfo
   {
      std::string topic;
      std::string json_ptr;
   };

   using Callback = std::function<void(std::size_t, float)>;

   ValueMqttCallback(MqttClient& mqtt_client, const std::vector<MessageInfo>& messages,
                     const Callback& update_callback = nullptr);
   ~ValueMqttCallback() override;

   const std::optional<float>& value(std::size_t index) const;

private:
   void connected() override;
   void messageArrived(const std::string& topic, const std::string& payload) override;

   MqttClient& m_client;
   std::vector<MessageInfo> m_messages;
   const Callback m_callback;
   std::vector<std::optional<float>> m_values;
   mutable std::mutex m_mutex;
};
