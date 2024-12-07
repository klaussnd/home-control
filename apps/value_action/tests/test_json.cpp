#include "../json_helper.h"

#include <gmock/gmock.h>

using namespace testing;

TEST(JsonHelper, readValueFromJson)
{
   const std::string message =
      R"({"temperature": 10.2, "humidity": 53, "pressure": 1011.1, "QNH": 1021.3})";
   const std::string json_ptr = "/temperature";

   const auto value = getValue(message, json_ptr);

   ASSERT_TRUE(value);
   ASSERT_THAT(value, Optional(FloatEq(10.2f)));
}

TEST(JsonHelper, readValueFromJsonNested)
{
   const std::string message = R"({"sensor": {"raw": 100, "converted": 43}})";
   const std::string json_ptr = "/sensor/raw";

   const auto value = getValue(message, json_ptr);

   ASSERT_TRUE(value);
   ASSERT_THAT(value, Optional(FloatEq(100.0f)));
}

TEST(JsonHelper, readValueDirect)
{
   const std::string message = "15.3";

   const auto value = getValue(message, {});

   ASSERT_TRUE(value);
   ASSERT_THAT(value, Optional(FloatEq(15.3f)));
}

TEST(JsonHelper, returnEmptyWhenNotANumber)
{
   const std::string message = "nonsense";

   const auto value = getValue(message, {});

   ASSERT_FALSE(value);
}

TEST(JsonHelper, returnEmptyWhenItemNotFound)
{
   const std::string message = R"({"QNH": 1021.3})";
   const std::string json_ptr = "/temperature";

   const auto value = getValue(message, json_ptr);

   ASSERT_FALSE(value);
}

TEST(JsonHelper, returnEmptyOnInvalidJson)
{
   const std::string invalid_message = R"({"sensor": {"nothing"})";
   const std::string json_ptr = "/sensor/nothing";

   const auto value = getValue(invalid_message, json_ptr);

   ASSERT_FALSE(value);
}

TEST(JsonHelper, returnEmptyOnInvalidValue)
{
   const std::string invalid_message = R"({"QNH": "nonesense"})";
   const std::string json_ptr = "/QNH";

   const auto value = getValue(invalid_message, json_ptr);

   ASSERT_FALSE(value);
}
