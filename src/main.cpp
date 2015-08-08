#include <iostream>
#include <png++/png.hpp>

using Pixel = png::rgb_pixel;
using Image = png::image<Pixel>;

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

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " [PNG FILENAME]" << std::endl;
    return EXIT_FAILURE;
  }
  try {
    auto image = Image(argv[1]);
    std::cout << codel_size(image) << std::endl;
  } catch (png::error& e) {
    std::cerr << e.what() << std::endl;
  }
  return EXIT_SUCCESS;
}
