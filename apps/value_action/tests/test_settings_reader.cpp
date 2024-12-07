#include "../settings_reader.h"

#include <gtest/gtest.h>

#include <gmock/gmock-matchers.h>

using testing::ElementsAre;
using testing::Eq;
using testing::FloatEq;
using testing::SizeIs;
using testing::StrEq;

bool operator==(const Action& lhs, const Action& rhs);

TEST(ASettingsReader, readsExampleFile)
{
   const std::string settings_file = "example.cfg";

   const auto settings = readSettings(settings_file);

   ASSERT_THAT(settings.mqtt_host, StrEq("mybroker-hostname"));

   ASSERT_THAT(settings.value_config, SizeIs(1));
   {
      const auto& item1 = settings.value_config[0];
      ASSERT_THAT(item1.name, StrEq("temperature some room"));
      ASSERT_THAT(item1.topic, StrEq("home/room"));
      ASSERT_THAT(item1.min, FloatEq(5.0f));
      ASSERT_THAT(item1.max, FloatEq(10.0f));
      ASSERT_THAT(item1.action_min, ElementsAre(Action{"home/room/heating", "ON"}));
      ASSERT_THAT(item1.action_max, ElementsAre(Action{"home/room/heating", "OFF"}));
   }
}

TEST(ASettingsReader, throwsIfNonExistentFile)
{
   ASSERT_THROW(readSettings("/non/existing/file"), std::runtime_error);
}

bool operator==(const Action& lhs, const Action& rhs)
{
   return lhs.topic == rhs.topic && lhs.payload == rhs.payload;
}
