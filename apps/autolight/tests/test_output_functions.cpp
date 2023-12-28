#include "../lamp_control.h"
#include "helper.h"
#include <gmock/gmock.h>

using namespace testing;

bool operator==(const OnOffTime& lhs, const OnOffTime& rhs);

class ShouldBeOn : public Test
{
public:
   void SetUp() override;

   LampSettings m_lamp_settings;
};

void ShouldBeOn::SetUp()
{
   m_lamp_settings = makeSettings();
}

using OnByAmbientLight = ShouldBeOn;

TEST_F(OnByAmbientLight, isOnWhenBelowThresholdWithPreviousUnknown)
{
   const float lightBelowThreshold = m_lamp_settings.ambient_light_threshold - 1.0f;
   EXPECT_THAT(shouldBeOnByAmbientLight(lightBelowThreshold, LampState::UNKNOWN,
                                        m_lamp_settings.ambient_light_threshold,
                                        m_lamp_settings.ambient_light_hysteresis),
               Eq(true));
}

TEST_F(OnByAmbientLight, isOnWhenAboveThresholdButBelowHysteresisWithPreviousOn)
{
   const float lightAboveThresholdBelowHystsresis =
      m_lamp_settings.ambient_light_threshold + 1.0f;
   EXPECT_THAT(shouldBeOnByAmbientLight(lightAboveThresholdBelowHystsresis, LampState::ON,
                                        m_lamp_settings.ambient_light_threshold,
                                        m_lamp_settings.ambient_light_hysteresis),
               Eq(true));
}

TEST_F(OnByAmbientLight, isOffWhenAboveThresholdAndHysteresisWithPreviousOn)
{
   const float lightAboveThresholdAndHysteresis =
      m_lamp_settings.ambient_light_threshold + m_lamp_settings.ambient_light_hysteresis
      + 1;
   EXPECT_THAT(shouldBeOnByAmbientLight(lightAboveThresholdAndHysteresis, LampState::ON,
                                        m_lamp_settings.ambient_light_threshold,
                                        m_lamp_settings.ambient_light_hysteresis),
               Eq(false));
}

TEST_F(OnByAmbientLight, isOffWhenAboveThresholdWithPreviousUnknown)
{
   const float lightAboveThreshold = m_lamp_settings.ambient_light_threshold + 1.0f;
   EXPECT_THAT(shouldBeOnByAmbientLight(lightAboveThreshold, LampState::UNKNOWN,
                                        m_lamp_settings.ambient_light_threshold,
                                        m_lamp_settings.ambient_light_hysteresis),
               Eq(false));
}

TEST_F(OnByAmbientLight, isOffWhenBelowThresholdButAboveHysteresisWithPreviousOff)
{
   const float lightBelowThresholdAboveHystsresis =
      m_lamp_settings.ambient_light_threshold - 1.0f;
   EXPECT_THAT(
      shouldBeOnByAmbientLight(lightBelowThresholdAboveHystsresis, LampState::OFF,
                               m_lamp_settings.ambient_light_threshold,
                               m_lamp_settings.ambient_light_hysteresis),
      Eq(false));
}

TEST_F(OnByAmbientLight, isOnWhenBelowThresholdAndHysteresisWithPreviousOff)
{
   const float lightBelowThresholdAndHysteresis =
      m_lamp_settings.ambient_light_threshold - m_lamp_settings.ambient_light_hysteresis
      - 1.0f;
   EXPECT_THAT(shouldBeOnByAmbientLight(lightBelowThresholdAndHysteresis, LampState::OFF,
                                        m_lamp_settings.ambient_light_threshold,
                                        m_lamp_settings.ambient_light_hysteresis),
               Eq(true));
}

class OnByTime : public ShouldBeOn
{
public:
   LampInfo m_lamp_status;
   std::mt19937 ran_gen;

   void initRandomConfig();
};

TEST_F(OnByTime, isOnWhenDuringOnTime)
{
   const std::time_t time = makeTime(Weekday::SUNDAY, 9, 0);

   ASSERT_THAT(shouldBeOnByTime(time, m_lamp_settings.timings, m_lamp_status, ran_gen),
               Eq(true));
}

TEST_F(OnByTime, isOffOnAnotherWeekdayDuringOnTime)
{
   const std::time_t time = makeTime(Weekday::MONDAY, 7, 0);

   ASSERT_THAT(shouldBeOnByTime(time, m_lamp_settings.timings, m_lamp_status, ran_gen),
               Eq(false));
}

TEST_F(OnByTime, isOffOutsideOnTime)
{
   const std::time_t time = makeTime(Weekday::SUNDAY, 5, 0);

   ASSERT_THAT(shouldBeOnByTime(time, m_lamp_settings.timings, m_lamp_status, ran_gen),
               Eq(false));
}

TEST_F(OnByTime, recomputesRandomTimesOnDayChange)
{
   initRandomConfig();
   const std::time_t time = makeTime(Weekday::SUNDAY, 0, 0);
   const auto yday = getYearDay(time);
   m_lamp_status.day_for_random = yday - 1;
   const auto previous_random_times = m_lamp_status.random_times;

   shouldBeOnByTime(time, m_lamp_settings.timings, m_lamp_status, ran_gen);

   ASSERT_THAT(m_lamp_status.day_for_random, Eq(yday));
   ASSERT_THAT(m_lamp_status.random_times,
               SizeIs(m_lamp_settings.timings[0].random->count));
   ASSERT_THAT(m_lamp_status.random_times, Ne(previous_random_times));
}

TEST_F(OnByTime, isOffOutsideRandomOnTime)
{
   initRandomConfig();
   const std::time_t time = makeTime(Weekday::SUNDAY, 9, 0);
   m_lamp_status.day_for_random = getYearDay(time);

   ASSERT_THAT(shouldBeOnByTime(time, m_lamp_settings.timings, m_lamp_status, ran_gen),
               Eq(false));
}

TEST_F(OnByTime, isOnInsideRandomOnTime)
{
   initRandomConfig();
   const std::time_t time = makeTime(Weekday::SUNDAY, 9, 6);
   m_lamp_status.day_for_random = getYearDay(time);

   ASSERT_THAT(shouldBeOnByTime(time, m_lamp_settings.timings, m_lamp_status, ran_gen),
               Eq(true));
}

void OnByTime::initRandomConfig()
{
   const RandomLampTime random_config{2u, 20u, 10u};
   m_lamp_settings.timings[0].random = random_config;
   m_lamp_status.random_times = {{8 * 60 + 10, 8 * 60 + 30}, {9 * 60 + 5, 9 * 60 + 25}};
}

bool operator==(const OnOffTime& lhs, const OnOffTime& rhs)
{
   return lhs.on == rhs.on && lhs.off == rhs.off;
}
