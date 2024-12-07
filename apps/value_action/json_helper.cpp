#include "json_helper.h"

#include <nlohmann/json.hpp>

#include <iostream>

namespace
{
std::optional<float> getValueFromJson(const std::string& message,
                                      const std::string& json_ptr);
}

std::optional<float> getValue(const std::string& message, const std::string& json_ptr)
{
   if (json_ptr.empty())
   {
      try
      {
         return std::stof(message);
      }
      catch (const std::exception&)
      {
         std::cout << "Unable to convert '" << message << "' to float" << std::endl;
         return {};
      }
   }
   else
   {
      return getValueFromJson(message, json_ptr);
   }
}

namespace
{
std::optional<float> getValueFromJson(const std::string& message,
                                      const std::string& json_ptr)
{
   try
   {
      const auto json = nlohmann::json::parse(message);
      try
      {
         return json.at(nlohmann::json::json_pointer{json_ptr}).get<float>();
      }
      catch (const nlohmann::json::exception& err)
      {
         std::cerr << "Unable to read value as float: message '" << message
                   << "' json pointer '" << json_ptr << "': " << err.what() << std::endl;
      }
   }
   catch (const nlohmann::json::exception& err)
   {
      std::cerr << "JSON error in '" << message << "': " << err.what() << std::endl;
   }
   return {};
}
}  // namespace
