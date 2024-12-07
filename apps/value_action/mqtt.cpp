#include "mqtt.h"

#include "json_helper.h"

#include <mqttbase/helper.h>

#include <algorithm>
#include <iostream>

namespace
{
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v);
}  // namespace

ValueMqttCallback::ValueMqttCallback(MqttClient& mqtt_client,
                                     const std::vector<MessageInfo>& messages,
                                     const Callback& update_callback)
      : m_client(mqtt_client)
      , m_messages(messages)
      , m_callback(update_callback)
      , m_values(messages.size())
{
}

ValueMqttCallback::~ValueMqttCallback() = default;

const std::optional<float>& ValueMqttCallback::value(std::size_t index) const
{
   std::scoped_lock<std::mutex> lock(m_mutex);
   return m_values[index];
}

void ValueMqttCallback::connected()
{
   std::cout << "connected" << std::endl;
   std::vector<std::string> topics;
   topics.reserve(m_messages.size());
   for (const auto& msg : m_messages)
   {
      if (std::find(topics.begin(), topics.end(), msg.topic) == topics.end())
      {
         topics.push_back(msg.topic);
      }
   }
   std::cout << "Subscribing to " << topics << std::endl;
   m_client.subscribe(topics);
}

void ValueMqttCallback::messageArrived(const std::string& topic,
                                       const std::string& payload)
{
   for (std::size_t index = 0; index < m_messages.size(); ++index)
   {
      const auto& msg = m_messages[index];
      if (topic == msg.topic)
      {
         if (const auto value = getValue(payload, msg.json_ptr); value.has_value())
         {
            {
               std::scoped_lock<std::mutex> lock(m_mutex);
               m_values[index] = value.value();
            }
            if (m_callback)
            {
               m_callback(index, value.value());
            }
         }
      }
   }
}

namespace
{
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
   bool first = true;
   for (const auto& item : v)
   {
      std::cout << (first ? "" : ", ") << item;
      first = false;
   }

   return os;
}
}  // namespace
