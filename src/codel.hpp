#pragma once
#include <cstddef>
#include <vector>
#include "utils.hpp"

std::size_t detect_codel_size(const Image &);

Color pixel_to_Color(const png::rgb_pixel &);

class CodelTable {
 public:
  CodelTable(const Image &image, const size_t codel_size) {
    size_t width = image.get_width() / codel_size;
    size_t height = image.get_height() / codel_size;
    table = std::vector<std::vector<Color>>(height, std::vector<Color>(width));
    for (size_t i = 0; i < height; ++i) {
      for (size_t j = 0; j < width; ++j) {
        auto pixel = image[i*codel_size][j*codel_size];
        table[i][j] = pixel_to_Color(pixel);
      }
    }
  }
  size_t height() const { return table.size(); }
  size_t width() const { return table[0].size(); }
  std::vector<Color> &operator[](const size_t i) { return table[i]; }
  const std::vector<Color> &operator[](const size_t i) const { return table[i]; }
  std::vector<std::vector<Color>> table;
};
