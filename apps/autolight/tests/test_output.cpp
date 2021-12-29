#include "../lamp_control.h"
#include "helper.h"

#include <gmock/gmock.h>

using namespace testing;

namespace
{
class OutputMock
{
public:
   MOCK_METHOD1(set, void(bool ison));
};
}  // namespace

class AnOutput : public Test
{
public:
   void SetUp() override;

   StrictMock<OutputMock> m_mock;
   LampSettings m_lamp_settings;
};

void AnOutput::SetUp()
{
   m_lamp_settings = makeSettings();
}

TEST_F(AnOutput, turnsOnWhenAmbientLightFallsBelowThresholdMinusHysteresis)
{
   EXPECT_CALL(m_mock, set(true));

   const time_t on_time = makeTime(Weekday::SUNDAY, 9, 0);
   const float ambientlight = m_lamp_settings.ambient_light_threshold
                              - m_lamp_settings.ambient_light_hysteresis - 1.0f;
   LampState previous_state = LampState::OFF;

   handleLamp(on_time, ambientlight, m_lamp_settings, previous_state,
              [&mock = m_mock](bool ison) { mock.set(ison); });

   EXPECT_THAT(previous_state, Eq(LampState::ON));
}

TEST_F(AnOutput, doesNotTurnOnAgainWhenAmbientLightIsBelowThresholdAndLightIsAlreadyOn)
{
   // expect no call of the mock
   EXPECT_CALL(m_mock, set(_)).Times(0);

   const time_t on_time = makeTime(Weekday::SUNDAY, 9, 0);
   const float ambientlight = m_lamp_settings.ambient_light_threshold
                              - m_lamp_settings.ambient_light_hysteresis - 1.0f;
   LampState previous_state = LampState::ON;

   handleLamp(on_time, ambientlight, m_lamp_settings, previous_state,
              [&mock = m_mock](bool ison) { mock.set(ison); });

   EXPECT_THAT(previous_state, Eq(LampState::ON));
}

TEST_F(AnOutput, turnsOffWhenAmbientLightFallsAboveThresholdPlusHysteresis)
{
   EXPECT_CALL(m_mock, set(false));

   const time_t on_time = makeTime(Weekday::SUNDAY, 9, 0);
   const float ambientlight = m_lamp_settings.ambient_light_threshold
                              + m_lamp_settings.ambient_light_hysteresis + 1;
   LampState previous_state = LampState::ON;

   handleLamp(on_time, ambientlight, m_lamp_settings, previous_state,
              [&mock = m_mock](bool ison) { mock.set(ison); });

   EXPECT_THAT(previous_state, Eq(LampState::OFF));
}
