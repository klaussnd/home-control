#pragma once

#include <mqttbase/MqttCallback.h>

template <std::size_t N>
class MultiMqttCallback : public MqttCallback
{
public:
   template <class... Item>
   MultiMqttCallback(Item&&... callbacks)
         : m_callbacks({std::forward<Item>(callbacks)...})
   {
   }
   ~MultiMqttCallback() override = default;

   void connected() override
   {
      for (auto& callback : m_callbacks)
      {
         callback.get().connected();
      }
   }

   void connectFailed() override
   {
      for (auto& callback : m_callbacks)
      {
         callback.get().connectFailed();
      }
   }

   void messageArrived(const std::string& topic, const std::string& value) override
   {
      for (auto& callback : m_callbacks)
      {
         callback.get().messageArrived(topic, value);
      }
   }

   void messagePublished(int id) override
   {
      for (auto& callback : m_callbacks)
      {
         callback.get().messagePublished(id);
      }
   }

private:
   std::array<std::reference_wrapper<MqttCallback>, N> m_callbacks;
};
