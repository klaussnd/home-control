#pragma once

#include <string>

class MqttCallback
{
public:
   virtual ~MqttCallback() = default;

   /// Called when the connection has been established
   virtual void connected()
   {
   }
   /// Called if connecting has failed
   virtual void connectFailed()
   {
   }
   /// Called when a new message has arrived
   virtual void messageArrived(const std::string& /*topic*/, const std::string& /*value*/)
   {
   }
   /// Called when a message has been published successfully
   /// @param id The message id of the sent message
   virtual void messagePublished(int /*id*/)
   {
   }
};
