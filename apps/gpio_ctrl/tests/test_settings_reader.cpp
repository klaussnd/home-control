#include "../settings_reader.h"

#include <gmock/gmock.h>

using namespace testing;

TEST(ASettingsReader, readsExampleFile)
{
   const std::string settings_file = "example.cfg";

   const auto settings = readSettings(settings_file);

   ASSERT_THAT(settings.mqtt_host, StrEq("mybroker-hostname"));
   ASSERT_THAT(settings.mqtt_topic_base, StrEq("home/room"));
   ASSERT_THAT(settings.mqtt_topic_suffix, StrEq("cmnd/POWER"));
   ASSERT_THAT(settings.gpio_chip, StrEq("gpiochip1"));

   ASSERT_THAT(settings.gpios, SizeIs(2));
   {
      const auto& gpio1 = settings.gpios[0];
      ASSERT_THAT(gpio1.name, StrEq("star"));
      ASSERT_THAT(gpio1.number, Eq(28));
   }
   {
      const auto& gpio2 = settings.gpios[1];
      ASSERT_THAT(gpio2.name, StrEq("nightlight"));
      ASSERT_THAT(gpio2.number, Eq(29));
   }
}

TEST(ASettingsReader, throwsIfNonExistentFile)
{
   ASSERT_THROW(readSettings("/non/existing/file"), std::runtime_error);
}
