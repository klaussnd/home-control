#include "../settings_reader.h"
#include "helper.h"

#include <gmock/gmock.h>

using namespace testing;

TEST(ASettingsReader, readsExampleFile)
{
   const std::string settings_file = "example.cfg";

   const auto settings = readSettings(settings_file);

   ASSERT_THAT(settings.mqtt_host, StrEq("mybroker-hostname"));
   ASSERT_THAT(settings.ambient_light_topic, StrEq("home/light"));
   ASSERT_THAT(settings.lamps, SizeIs(2));

   {
      const auto& lamp1 = settings.lamps[0];
      ASSERT_THAT(lamp1.name, StrEq("stairs"));
      ASSERT_THAT(lamp1.topic, StrEq("home/stairs/cmnd/POWER2"));
      ASSERT_THAT(lamp1.ambient_light_threshold, FloatEq(960.0f));
      ASSERT_THAT(lamp1.ambient_light_hysteresis, FloatEq(7.0f));
      ASSERT_THAT(lamp1.timings, SizeIs(2));
      ASSERT_THAT(lamp1.timings[0].weekday,
                  Eq(Weekday::MONDAY | Weekday::TUESDAY | Weekday::WEDNESDAY
                     | Weekday::THURSDAY | Weekday::FRIDAY));
      ASSERT_THAT(lamp1.timings[0].on, Eq(5 * 60 + 30));
      ASSERT_THAT(lamp1.timings[0].off, Eq(22 * 60));
      ASSERT_THAT(lamp1.timings[1].weekday, Eq(Weekday::SATURDAY | Weekday::SUNDAY));
      ASSERT_THAT(lamp1.timings[1].on, Eq(6 * 60));
      ASSERT_THAT(lamp1.timings[1].off, Eq(23 * 60));
   }
   {
      const auto& lamp2 = settings.lamps[1];
      ASSERT_THAT(lamp2.name, StrEq("outside"));
      ASSERT_THAT(lamp2.topic, StrEq("home/outside/lamp/cmnd/POWER1"));
      ASSERT_THAT(lamp2.ambient_light_threshold, FloatEq(950.0f));
      ASSERT_THAT(lamp2.ambient_light_hysteresis, FloatEq(7.0f));
      ASSERT_THAT(lamp2.timings, SizeIs(1));
      ASSERT_THAT(lamp2.timings[0].weekday, Eq(Weekday::THURSDAY | Weekday::SATURDAY));
      ASSERT_THAT(lamp2.timings[0].on, Eq(7 * 60 + 15));
      ASSERT_THAT(lamp2.timings[0].off, Eq(22 * 60));
   }
}

TEST(ASettingsReader, throwsIfNonExistentFile)
{
   ASSERT_THROW(readSettings("/non/existing/file"), std::runtime_error);
}
