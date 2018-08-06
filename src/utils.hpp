#pragma once
#include <vector>
#include <png++/png.hpp>

using Pixel = png::rgb_pixel;
using Image = png::image<Pixel>;
using byte = png::byte;

enum class Hue {
  BLACK, RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA, WHITE, UNKNOWN
};

enum class Brightness {
  WHITE, LIGHT, NORMAL, DARK, BLACK, UNKNOWN
};

struct Color {
  Hue hue;
  Brightness brightness;
};

bool is_black(const Color &);
bool is_white(const Color &);
bool is_black_or_white(const Color &);
bool is_color(const Color &);
bool is_valid(const Color &);
bool operator==(const Color &, const Color &);
bool operator!=(const Color &, const Color &);
Color unknown_color();

class CodelsTable {
 public:
  CodelsTable(const std::size_t height, const std::size_t width)
    : data(height, std::vector<Color>(width)) {}
  Color set(const std::size_t i, const std::size_t j, const Color& color) {
    return data.at(i).at(j) = color;
  }
  Color get(const std::size_t i, const std::size_t j) const {
    return data.at(i).at(j);
  }
  std::size_t height() const { return data.size(); }
  std::size_t width() const { return data.front().size(); }
 private:
  std::vector<std::vector<Color>> data;
};
