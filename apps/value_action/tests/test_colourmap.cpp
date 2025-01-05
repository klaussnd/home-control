#include "../linearcolourmap.h"

#include <gmock/gmock.h>

using namespace testing;

TEST(AColourmapGreenRed, mapsZeroToGreen)
{
   const auto rgb = mapColourLinear(0.0f, ColourMapType::GREEN_RED, 1.0f);

   ASSERT_THAT(rgb, FieldsAre(0u, 255u, 0u));
}

TEST(AColourmapGreenRed, mapsZeroToGreenWithBrightnessHalf)
{
   const auto rgb = mapColourLinear(0.0f, ColourMapType::GREEN_RED, 0.5f);

   ASSERT_THAT(rgb, FieldsAre(0u, 128u, 0u));
}

TEST(AColourmapGreenRed, mapsOneToRed)
{
   const auto rgb = mapColourLinear(1.0f, ColourMapType::GREEN_RED, 1.0f);

   ASSERT_THAT(rgb, FieldsAre(255u, 0u, 0u));
}

TEST(AColourmapGreenRed, mapsHalfToYellow)
{
   const auto rgb = mapColourLinear(0.5f, ColourMapType::GREEN_RED, 1.0f);

   ASSERT_THAT(rgb, FieldsAre(255u, 255u, 0u));
}

TEST(AColourmapGreenRed, mapsQuarterToGreenYellow)
{
   const auto rgb = mapColourLinear(0.25f, ColourMapType::GREEN_RED, 1.0f);

   ASSERT_THAT(rgb, FieldsAre(128u, 255u, 0u));
}

TEST(AColourmapGreenRed, mapsQuarterToGreenYellowWithBrghtnessHalf)
{
   const auto rgb = mapColourLinear(0.25f, ColourMapType::GREEN_RED, 0.5f);

   ASSERT_THAT(rgb, FieldsAre(64u, 128u, 0u));
}

TEST(AColourmapGreenRed, mapsValuesSmallerThanZeroToGreen)
{
   const auto rgb = mapColourLinear(-0.5f, ColourMapType::GREEN_RED, 1.0f);

   ASSERT_THAT(rgb, FieldsAre(0u, 255u, 0u));
}

TEST(AColourmapGreenRed, mapsValuesLargerThanOneToRed)
{
   const auto rgb = mapColourLinear(1.5f, ColourMapType::GREEN_RED, 1.0f);

   ASSERT_THAT(rgb, FieldsAre(255, 0u, 0u));
}
