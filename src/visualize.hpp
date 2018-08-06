#pragma once
#include "utils.hpp"

using Pixel = png::rgb_pixel;
using Image = png::image<Pixel>;
using byte = png::byte;

Hue what_hue(const Pixel &);
Brightness what_lightness(const Pixel &);
Color what_color(const Pixel &);

void visualize(const Image &);
