#pragma once

#include <mqttbase/MqttCallback.h>

#include <memory>
#include <ostream>
#include <string>
#include <vector>

struct mosquitto;

struct MqttClientSettings
{
   /** The host to connect to.
       SSL certificate is searched for in a standard location based on the hostname:
       ${HOME}/CERTIFICATE_FOLDER/hostname.crt
       If no certificate for the hostname can be found, a connection without SSL is used.
    */
   std::string host;
   /// Username for authentication on the broken, empty to disable
   std::string username;
   /// Password for authenticating on the broker, empty to disable
   std::string password;
   /** A client identifier that is unique on the server.
       The hostname is automatically added to this string to make it more unique. */
   std::string id;
   /// A prefix prepended to all topics on subscribe or publish
   std::string topic_prefix;
};

class MqttClient
{
public:
   MqttClient(MqttClientSettings settings);
   ~MqttClient();

   bool connectAsync()
   {
      return connect(ConnectType::ASYNC);
   }
   bool connectWait()
   {
      return connect(ConnectType::WAIT);
   }

   void publish(const std::string& topic, const std::string& value);
   void publish(const std::string& topic, float value)
   {
      publish(topic, std::to_string(value));
   }
   void publish(const std::string& topic, int value)
   {
      publish(topic, std::to_string(value));
   }
   void publish(const std::string& topic)  ///< without value
   {
      publish(topic, std::string{});
   }

   bool subscribe(const std::string& topic);
   bool subscribe(const std::vector<std::string>& topics);

   void setCallback(MqttCallback& cb);

protected:
   enum class ConnectType
   {
      WAIT,  ///< wait until the connection is established
      ASYNC  ///< connect asynchronously
   };

   bool connect(ConnectType how);
   std::string fullTopic(const std::string& leanTopic);
   std::string leanTopic(const std::string& fullTopic);

   static void mosquittoConnectCallback(struct mosquitto* mosq, void* userdata,
                                        int result);
   static void mosquittoMessageCallback(struct mosquitto* mosq, void* userdata,
                                        const struct mosquitto_message* message);
   static void mosquittoPublishCallback(struct mosquitto* mosq, void* userdata, int id);

protected:
   std::unique_ptr<struct mosquitto, void (*)(struct mosquitto*)> m_mosquitto;
   MqttCallback* m_callback;
   std::string m_host;
   std::string m_topic_prefix;
   std::ostream& m_logger;
};
