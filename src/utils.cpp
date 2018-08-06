#include "utils.hpp"

bool is_black(const Color &color) {
  return color.brightness == Brightness::BLACK && color.hue == Hue::BLACK;
}

bool is_white(const Color &color) {
  return color.brightness == Brightness::WHITE && color.hue == Hue::WHITE;
}

bool is_black_or_white(const Color &color) {
  return is_black(color) || is_white(color);
}

bool is_color_brightness(const Brightness &brightness) {
  return brightness != Brightness::UNKNOWN && brightness != Brightness::BLACK && brightness != Brightness::WHITE;
}

bool is_color_hue(const Hue &hue) {
  return hue != Hue::UNKNOWN && hue != Hue::BLACK && hue != Hue::WHITE;
}

bool is_color(const Color &color) {
  return is_color_brightness(color.brightness) && is_color_hue(color.hue);
}

bool is_valid(const Color &color) {
  return is_black_or_white(color) && is_color(color);
}

bool operator==(const Color &lhs, const Color &rhs) {
  return lhs.hue == rhs.hue && lhs.brightness == rhs.brightness;
}

bool operator!=(const Color &lhs, const Color &rhs) {
  return lhs.hue != rhs.hue || lhs.brightness != rhs.brightness;
}

Color unknown_color() {
  return Color { Hue::UNKNOWN, Brightness::UNKNOWN };
}
