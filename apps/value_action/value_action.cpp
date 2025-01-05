#include "linearcolourmap.h"
#include "mqtt.h"
#include "settings.h"
#include "settings_reader.h"
#include <mqttbase/MqttClient.h>
#include <mqttbase/helper.h>

#include <cerrno>
#include <csignal>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

enum class State
{
   UNKNOWN,
   LOW,  ///< value lower than minimum
   IN_BETWEEN,
   HIGH,  ///< value higher than maximum
};

class ValueAction
{
public:
   ValueAction(const std::vector<ValueConfig>& config, MqttClient& mqtt_client);
   void updateValue(std::size_t index, float value);

private:
   void doAction(const std::vector<Action>& actions);
   void doVisualisation(const Visualisation& visualisation, float value, float min,
                        float max);

   const std::vector<ValueConfig>& m_config;
   std::vector<State> m_state;  ///< state when last action was performed, for each config
   MqttClient& m_mqtt;
};

volatile bool run = true;

std::vector<ValueMqttCallback::MessageInfo> makeMessageInfo(
   const std::vector<ValueConfig>& config);
std::string toString(State state);
void replace(std::string& str, const std::string& to_replace,
             const std::string& replace_with);
void siginthandler(int signo);

int main(void)
{
   if (::signal(SIGINT, siginthandler) == SIG_ERR)
   {
      std::cerr << "Unable to register signal handler: " << strerror(errno) << std::endl;
   }

   try
   {
      const std::string settingsFile = homeDirectory() + "/.config/value_action.cfg";
      const auto settings = readSettings(settingsFile);
      std::cout << "Configuration " << settingsFile << " contains "
                << settings.value_config.size() << " actions." << std::endl;

      MqttClient mqtt{MqttClientSettings{settings.mqtt_host, {}, {}, "value_action", ""}};
      ValueAction action{settings.value_config, mqtt};
      ValueMqttCallback callback{mqtt, makeMessageInfo(settings.value_config),
                                 std::bind(&ValueAction::updateValue, &action,
                                           std::placeholders::_1, std::placeholders::_2)};
      mqtt.setCallback(callback);
      std::cout << "Connecting to " << settings.mqtt_host << " ... " << std::flush;
      mqtt.connectAsync();

      while (run)
      {
         std::this_thread::sleep_for(std::chrono::milliseconds(200));
      }
   }
   catch (const std::exception& err)
   {
      std::cerr << "Error: " << err.what() << std::endl;
      return EXIT_FAILURE;
   }

   std::cout << "Exit." << std::endl;
   return EXIT_SUCCESS;
}

ValueAction::ValueAction(const std::vector<ValueConfig>& config, MqttClient& mqtt_client)
      : m_config(config)
      , m_state(config.size(), State::UNKNOWN)
      , m_mqtt(mqtt_client)
{
}

void ValueAction::updateValue(std::size_t index, float value)
{
   const auto& config = m_config[index];
   const auto new_state = value < config.min   ? State::LOW
                          : value > config.max ? State::HIGH
                                               : State::IN_BETWEEN;

   if ((new_state == State::LOW || new_state == State::HIGH)
       && new_state != m_state[index])
   {
      std::cout << "Execute action for " << toString(new_state) << " "
                << toString(new_state) << std::endl;
      doAction(new_state == State::LOW ? config.action_min : config.action_max);
      m_state[index] = new_state;
   }

   if (config.visualisation.has_value())
   {
      doVisualisation(config.visualisation.value(), value, config.min, config.max);
   }
}

void ValueAction::doAction(const std::vector<Action>& actions)
{
   for (const auto& action : actions)
   {
      std::cout << "Publish " << action.topic << " value " << action.payload << std::endl;
      m_mqtt.publish(action.topic, action.payload);
   }
}

void ValueAction::doVisualisation(const Visualisation& visualisation, float value,
                                  float min, float max)
{
   const float normalised_value = std::clamp((value - min) / (max - min), 0.0f, 1.0f);
   const auto rgb = mapColourLinear(normalised_value, visualisation.colour_map_type,
                                    visualisation.brightness_percent / 100.f);
   std::string payload = visualisation.payload;
   replace(payload, "%r", std::to_string(rgb.red));
   replace(payload, "%g", std::to_string(rgb.green));
   replace(payload, "%b", std::to_string(rgb.blue));
   m_mqtt.publish(visualisation.topic, payload);
}

std::vector<ValueMqttCallback::MessageInfo> makeMessageInfo(
   const std::vector<ValueConfig>& config)
{
   std::vector<ValueMqttCallback::MessageInfo> messages;
   messages.reserve(config.size());
   for (const auto& datum : config)
   {
      messages.emplace_back(ValueMqttCallback::MessageInfo{datum.topic, datum.json_ptr});
   }
   return messages;
}

std::string toString(State state)
{
   switch (state)
   {
   case State::UNKNOWN:
      return "unknown";
   case State::LOW:
      return "low";
   case State::IN_BETWEEN:
      return "in between";
   case State::HIGH:
      return "high";
   }
   return "?";
}

void replace(std::string& str, const std::string& to_replace,
             const std::string& replace_with)
{
   const std::size_t pos = str.find(to_replace);
   if (pos != std::string::npos)
   {
      str.replace(pos, to_replace.length(), replace_with);
   }
}

void siginthandler(int)
{
   if (run == false)
      exit(EXIT_FAILURE);
   else
      run = false;
}
