#include "visualize.hpp"


/*
Color how_bright(const Pixel& pixel) {
  const byte r = pixel.red, g = pixel.green, b = pixel.blue;
  const byte c = 0xC0;
  const auto mini = std::min(r, std::min(g, b));
  const auto maxi = std::max(r, std::max(g, b));
  if (mini == c) {
    return Color::WHITE;
  } else if (maxi == c) {
    return Color::BLACK;
  } else {
    return what_color(pixel);
  }
}
*/

/*
void show_pixel(const Pixel& pixel) {
  static const char* fore[] =
      {"\x1b[30;1m", "\x1b[31;1m", "\x1b[32;1m", "\x1b[33;1m",
       "\x1b[34;1m", "\x1b[35;1m", "\x1b[36;1m", "\x1b[37;1m"};
  static const char* back[] =
      {"\x1b[40;1m", "\x1b[41;1m", "\x1b[42;1m", "\x1b[43;1m",
       "\x1b[44;1m", "\x1b[45;1m", "\x1b[46;1m", "\x1b[47;1m"};
  const auto reset = "\x1b[0m";
  static const char text[] = {'K', 'R', 'G', 'Y', 'B', 'M', 'C', 'W'};
  const auto color = what_color(pixel);
  const auto bright = what_lightness(pixel);
  if (color == Color::UNKNOWN) {
    std::cout << "?";
  } else {
    const auto c = static_cast<int>(color);
    const auto b = static_cast<int>(bright);
    std::cout << back[c] << fore[b] << text[c] << reset;
  }
}
*/

size_t gcd(size_t a, size_t b) {
  if (b == 0) {
    return a;
  } else {
    return gcd(b, a % b);
  }
}

bool same_pixel(const Pixel& lhs, const Pixel& rhs) {
  return lhs.red == rhs.red && lhs.green == rhs.green && lhs.blue == rhs.blue;
}

size_t codel_size(const Image& image) {
  const size_t width = image.get_width();
  const size_t height = image.get_height();
  Pixel prev;
  size_t minimum = gcd(width, height);
  size_t count = 0;
  // vertical iterate
  for (size_t row = 0; row < height; ++row) {
    for (size_t col = 0; col < width; ++col) {
      const auto pixel = image[row][col];
      if (0 < count && !same_pixel(prev, pixel)) {
        minimum = gcd(minimum, count);
        count = 0;
      }
      ++count;
      prev = pixel;
    }
  }
  count = 0;
  // horizontal iterate
  for (size_t col = 0; col < width; ++col) {
    for (size_t row = 0; row < height; ++row) {
      const auto pixel = image[row][col];
      if (0 < count && !same_pixel(prev, pixel)) {
        minimum = gcd(minimum, count);
        count = 0;
      }
      ++count;
      prev = pixel;
    }
  }
  return minimum;
}

/*
void visualize(const Image& image) {
  const auto step = codel_size(image);
  for (size_t row = 0; row < image.get_height(); row += step) {
    for (size_t col = 0; col < image.get_width(); col += step) {
      show_pixel(image[row][col]);
    }
    std::cout << std::endl;
  }
}
*/
