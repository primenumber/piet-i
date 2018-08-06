#include "codel.hpp"

Color pixel_to_Color(const png::rgb_pixel &pixel) {
  auto red = pixel.red;
  auto green = pixel.green;
  auto blue = pixel.blue;
  int red_p, green_p, blue_p;
  switch (red) {
    case 0: red_p = 0; break;
    case 0xC0: red_p = 1; break;
    case 0xFF: red_p = 2; break;
    default: return unknown_color();
  }
  switch (green) {
    case 0: green_p = 0; break;
    case 0xC0: green_p = 1; break;
    case 0xFF: green_p = 2; break;
    default: return unknown_color();
  }
  switch (blue) {
    case 0: blue_p = 0; break;
    case 0xC0: blue_p = 1; break;
    case 0xFF: blue_p = 2; break;
    default: return unknown_color();
  }
  int all_p = red_p + green_p * 3 + blue_p * 9;
  switch (all_p) {
    case 0: // 000000
      return Color { Hue::BLACK, Brightness::BLACK };
    case 1: // C00000
      return Color { Hue::RED, Brightness::DARK };
    case 2: // FF0000
      return Color { Hue::RED, Brightness::NORMAL };
    case 3: // 00C000
      return Color { Hue::GREEN, Brightness::DARK };
    case 4: // C0C000
      return Color { Hue::YELLOW, Brightness::DARK };
    case 6: // 00FF00
      return Color { Hue::GREEN, Brightness::NORMAL };
    case 8: // FFFF00
      return Color { Hue::YELLOW, Brightness::NORMAL };
    case 9: // 0000C0
      return Color { Hue::BLUE, Brightness::DARK };
    case 10: // C000C0
      return Color { Hue::MAGENTA, Brightness::DARK };
    case 12: // 00C0C0
      return Color { Hue::CYAN, Brightness::DARK };
    case 14: // FFC0C0
      return Color { Hue::RED, Brightness::LIGHT };
    case 16: // C0FFC0
      return Color { Hue::GREEN, Brightness::LIGHT };
    case 17: // FFFFC0
      return Color { Hue::YELLOW, Brightness::LIGHT };
    case 18: // 0000FF
      return Color { Hue::BLUE, Brightness::NORMAL };
    case 20: // FF00FF
      return Color { Hue::MAGENTA, Brightness::NORMAL };
    case 22: // C0C0FF
      return Color { Hue::BLUE, Brightness::LIGHT };
    case 23: // FFC0FF
      return Color { Hue::MAGENTA, Brightness::LIGHT };
    case 24: // 00FFFF
      return Color { Hue::CYAN, Brightness::NORMAL };
    case 25: // C0FFFF
      return Color { Hue::CYAN, Brightness::LIGHT };
    case 26: // FFFFFF
      return Color { Hue::WHITE, Brightness::WHITE };
    default:
      return unknown_color();
  }
}
