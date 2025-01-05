#include "linearcolourmap.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>  // isnan()

namespace
{
// Colormap definition.
static constexpr std::array<Rgb, 3> m_colours_green_red = {
   {{0x00, 0xff, 0x00}, {0xff, 0xff, 0x00}, {0xff, 0x00, 0x00}}};
static constexpr std::array<Rgb, 3> m_colours_blue_red = {
   {{0x00, 0x00, 0xff}, {0xff, 0x00, 0xff}, {0xff, 0x00, 0x00}}};
}  // namespace

Rgb mapColourLinear(float normalised_value, ColourMapType colour_map_type,
                    float brightness)
{
   const auto& colours = colour_map_type == ColourMapType::BLUE_RED ? m_colours_blue_red
                                                                    : m_colours_green_red;
   const bool inverse = colour_map_type == ColourMapType::RED_GREEN;

   // Linearly interpolate between colours.
   normalised_value = std::clamp(normalised_value, 0.0f, 1.0f);
   if (inverse)
   {
      normalised_value = 1.0 - normalised_value;
   }
   normalised_value *= (static_cast<float>(colours.size()) - 1.0f);

   const unsigned int segment = static_cast<unsigned int>(normalised_value);
   assert(segment < colours.size());
   const float factor = normalised_value - static_cast<float>(segment);

   Rgb result;

   float interp =
      factor * static_cast<float>(colours[segment + 1].red - colours[segment].red);
   result.red = std::round((colours[segment].red + interp) * brightness);

   interp =
      factor * static_cast<float>(colours[segment + 1].green - colours[segment].green);
   result.green = std::round((colours[segment].green + interp) * brightness);

   interp =
      factor * static_cast<float>(colours[segment + 1].blue - colours[segment].blue);
   result.blue = std::round((colours[segment].blue + interp) * brightness);

   return result;
}
