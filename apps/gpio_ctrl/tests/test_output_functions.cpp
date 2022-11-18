#include "../lamp_control.h"
#include "helper.h"
#include <gmock/gmock.h>

using namespace testing;

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

using OnByTime = ShouldBeOn;

TEST_F(OnByTime, isOnWhenDuringOnTime)
{
   const std::time_t time = makeTime(Weekday::SUNDAY, 9, 0);
   ASSERT_THAT(shouldBeOnByTime(time, m_lamp_settings.timings), Eq(true));
}

TEST_F(OnByTime, isOffOnAnotherWeekdayDuringOnTime)
{
   const std::time_t time = makeTime(Weekday::MONDAY, 7, 0);
   ASSERT_THAT(shouldBeOnByTime(time, m_lamp_settings.timings), Eq(false));
}

TEST_F(OnByTime, isOffOutsideOnTime)
{
   const std::time_t time = makeTime(Weekday::SUNDAY, 5, 0);
   ASSERT_THAT(shouldBeOnByTime(time, m_lamp_settings.timings), Eq(false));
}
