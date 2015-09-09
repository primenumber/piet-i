#pragma once
#include <png++/png.hpp>

using Pixel = png::rgb_pixel;
using Image = png::image<Pixel>;
using byte = png::byte;

enum class Color {
  BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, UNKNOWN
};

void visualize(const Image&);
