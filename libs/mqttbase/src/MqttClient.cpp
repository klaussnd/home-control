#include <mqttbase/MqttClient.h>
#include <mqttbase/helper.h>

#include <mosquitto.h>

#include <filesystem>
#include <iostream>

#ifndef CERTIFICATE_FOLDER
#define CERTIFICATE_FOLDER ".certificates"
#endif

namespace
{
std::string certificateFilePath(const std::string& hostname);

constexpr int QOS = 1;  // quality of service
constexpr int KEEPALIVE_TIME = 60;
}  // namespace

MqttClient::MqttClient(MqttClientSettings settings)
      : m_mosquitto(nullptr, mosquitto_destroy)
      , m_callback(nullptr)
      , m_host(std::move(settings.host))
      , m_topic_prefix(std::move(settings.topic_prefix))
      , m_logger(std::cout)
{
   mosquitto_lib_init();
   m_mosquitto.reset(mosquitto_new(nullptr, true, this));

   if (!settings.username.empty())
   {
      const auto res = mosquitto_username_pw_set(
         m_mosquitto.get(), settings.username.c_str(), settings.password.c_str());
      if (res != MOSQ_ERR_SUCCESS)
      {
         throw std::runtime_error("Unable to set username/password: "
                                  + std::to_string(res));
      }
   }

   const auto certificate_path = certificateFilePath(m_host);
   const bool use_ssl = std::filesystem::exists(certificate_path);
   if (use_ssl)
   {
      const auto res = mosquitto_tls_set(m_mosquitto.get(), certificate_path.c_str(),
                                         nullptr, nullptr, nullptr, nullptr);
      if (res != MOSQ_ERR_SUCCESS)
      {
         throw std::runtime_error("Unable to set SSL: " + std::to_string(res));
      }
   }

   mosquitto_loop_start(m_mosquitto.get());  // start network actions in a separate thread
}

MqttClient::~MqttClient()
{
   mosquitto_disconnect(m_mosquitto.get());
   mosquitto_loop_stop(m_mosquitto.get(), false);
   m_mosquitto.reset(nullptr);
   mosquitto_lib_cleanup();
}

bool MqttClient::connect(ConnectType how)
{
   const auto certificate_path = certificateFilePath(m_host);
   const bool use_ssl = std::filesystem::exists(certificate_path);
   const auto fqdn = getFqdn(m_host);
   const int port = use_ssl ? 8883 : 1883;
   const int res =
      how == ConnectType::ASYNC
         ? mosquitto_connect_async(m_mosquitto.get(), fqdn.c_str(), port, KEEPALIVE_TIME)
         : mosquitto_connect(m_mosquitto.get(), fqdn.c_str(), port, KEEPALIVE_TIME);
   return res == MOSQ_ERR_SUCCESS;
}

void MqttClient::publish(const std::string& topic, const std::string& value)
{
   mosquitto_publish(m_mosquitto.get(), nullptr, fullTopic(topic).c_str(),
                     static_cast<int>(value.size()),
                     reinterpret_cast<const void*>(value.c_str()), QOS, false);
}

bool MqttClient::subscribe(const std::string& topic)
{
   return mosquitto_subscribe(m_mosquitto.get(), nullptr, fullTopic(topic).c_str(), 0)
          == MOSQ_ERR_SUCCESS;
}

bool MqttClient::subscribe(const std::vector<std::string>& topics)
{
   std::vector<std::string> full_topics(topics.size());
   char* topic_ptr[topics.size()];
   for (std::size_t i = 0; i < full_topics.size(); ++i)
   {
      full_topics[i] = fullTopic(topics[i]);
      topic_ptr[i] = full_topics[i].data();
   }
   return mosquitto_subscribe_multiple(m_mosquitto.get(), nullptr,
                                       static_cast<int>(topics.size()), topic_ptr, 0, 0,
                                       nullptr)
          == MOSQ_ERR_SUCCESS;
}

void MqttClient::setCallback(MqttCallback& cb)
{
   m_callback = &cb;
   mosquitto_connect_callback_set(m_mosquitto.get(),
                                  &MqttClient::mosquittoConnectCallback);
   mosquitto_message_callback_set(m_mosquitto.get(),
                                  &MqttClient::mosquittoMessageCallback);
   mosquitto_publish_callback_set(m_mosquitto.get(),
                                  &MqttClient::mosquittoPublishCallback);
}

std::string MqttClient::fullTopic(const std::string& lean_topic)
{
   return m_topic_prefix.empty() ? lean_topic
          : lean_topic.empty()   ? m_topic_prefix
                                 : m_topic_prefix + '/' + lean_topic;
}

std::string MqttClient::leanTopic(const std::string& full_topic)
{
   if (!m_topic_prefix.empty() && full_topic.find(m_topic_prefix + "/") == 0)
   {
      return full_topic.substr(m_topic_prefix.size() + 1);
   }

   return full_topic;
}

void MqttClient::mosquittoConnectCallback(struct mosquitto*, void* userdata, int result)
{
   MqttClient* client = reinterpret_cast<MqttClient*>(userdata);
   if (client->m_callback)
   {
      if (result == 0)
      {
         client->m_callback->connected();
      }
      else
      {
         client->m_callback->connectFailed();
      }
   }
}

void MqttClient::mosquittoMessageCallback(struct mosquitto*, void* userdata,
                                          const struct mosquitto_message* message)
{
   MqttClient* client = reinterpret_cast<MqttClient*>(userdata);
   if (client->m_callback)
   {
      client->m_callback->messageArrived(
         client->leanTopic(message->topic),
         message->payload ? std::string{reinterpret_cast<char*>(message->payload)}
                          : std::string{});
   }
}

void MqttClient::mosquittoPublishCallback(struct mosquitto*, void* userdata, int id)
{
   MqttClient* client = reinterpret_cast<MqttClient*>(userdata);
   if (client->m_callback)
   {
      client->m_callback->messagePublished(id);
   }
}

namespace
{
std::string certificateFilePath(const std::string& hostname)
{
   return homeDirectory() + '/' + CERTIFICATE_FOLDER + '/' + hostname + ".crt";
}
}  // namespace
