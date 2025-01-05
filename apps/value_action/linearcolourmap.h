#pragma once

#include <cstdint>

struct Rgb
{
   uint8_t red;
   uint8_t green;
   uint8_t blue;
};

enum class ColourMapType
{
   GREEN_RED,
   RED_GREEN,
   BLUE_RED,
};

/// Returns a colour representation of a value between 0 and 1
/// @param normalised_value  A value between 0 and 1
/// @param colour_map_type   The colour map to use
/// @param brightness        The brightness of the colour, value between 0 and 1
Rgb mapColourLinear(float normalised_value, ColourMapType colour_map_type,
                    float brightness);
