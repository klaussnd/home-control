#include "../settings_reader.h"

#include <gtest/gtest.h>

#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>

using testing::ElementsAre;
using testing::Eq;
using testing::FloatEq;
using testing::IsEmpty;
using testing::Optional;
using testing::SizeIs;
using testing::StrEq;

bool operator==(const Action& lhs, const Action& rhs);
bool operator==(const Visualisation& lhs, const Visualisation& rhs);

void PrintTo(const Visualisation& v, std::ostream* os);
std::string toString(ColourMapType colour_map_type);

TEST(ASettingsReader, readsExampleFile)
{
   const std::string settings_file = "example.cfg";

   const auto settings = readSettings(settings_file);

   ASSERT_THAT(settings.mqtt_host, StrEq("mybroker-hostname"));

   ASSERT_THAT(settings.value_config, SizeIs(2));
   {
      const auto& item1 = settings.value_config[0];
      ASSERT_THAT(item1.name, StrEq("temperature some room"));
      ASSERT_THAT(item1.topic, StrEq("home/room"));
      ASSERT_THAT(item1.min, FloatEq(5.0f));
      ASSERT_THAT(item1.max, FloatEq(10.0f));
      ASSERT_THAT(item1.action_min, ElementsAre(Action{"home/room/heating", "ON"}));
      ASSERT_THAT(item1.action_max, ElementsAre(Action{"home/room/heating", "OFF"}));
      ASSERT_THAT(item1.visualisation, Eq(std::nullopt));
   }
   {
      const auto& item2 = settings.value_config[1];
      ASSERT_THAT(item2.name, StrEq("temperature other room"));
      ASSERT_THAT(item2.topic, StrEq("home/other_room"));
      ASSERT_THAT(item2.min, FloatEq(5.0f));
      ASSERT_THAT(item2.max, FloatEq(20.0f));
      ASSERT_THAT(item2.action_min, IsEmpty());
      ASSERT_THAT(item2.action_max, IsEmpty());
      ASSERT_THAT(item2.visualisation,
                  Optional(Visualisation{"home/temp_visualisation", "%r,%g,%b",
                                         ColourMapType::BLUE_RED, 50u}));
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

bool operator==(const Visualisation& lhs, const Visualisation& rhs)
{
   return lhs.topic == rhs.topic && lhs.payload == rhs.payload
          && lhs.colour_map_type == rhs.colour_map_type
          && lhs.brightness_percent == rhs.brightness_percent;
}

void PrintTo(const Visualisation& v, std::ostream* os)
{
   *os << "{ topic " << v.topic << " payload " << v.payload << " colour map "
       << toString(v.colour_map_type) << " brightness " << v.brightness_percent << "%}";
}

std::string toString(ColourMapType colour_map_type)
{
   switch (colour_map_type)
   {
   case ColourMapType::GREEN_RED:
      return "green_red";
   case ColourMapType::RED_GREEN:
      return "red_green";
   case ColourMapType::BLUE_RED:
      return "blue_red";
   }

   return "<unknown type> " + std::to_string(static_cast<int>(colour_map_type));
}
