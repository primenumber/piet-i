#include "parser.hpp"

Hue what_hue(const Pixel &pixel) {
  const byte r = pixel.red, g = pixel.green, b = pixel.blue;
  const byte z = 0x00, c = 0xC0, f = 0xFF;
  if ((r == z || r == c || r == f) &&
      (g == z || g == c || g == f) &&
      (b == z || b == c || b == f)) {
    if (r == z && g == z && b == z) {
      return Hue::BLACK;
    } else if (r == f && g == f && b == f) {
      return Hue::WHITE;
    } else if (g == z && b == z) {
      return Hue::RED;
    } else if (r == z && b == z) {
      return Hue::GREEN;
    } else if (r == z && g == z) {
      return Hue::BLUE;
    } else if (r == f && g == f) {
      return Hue::YELLOW;
    } else if (g == f && b == f) {
      return Hue::CYAN;
    } else if (r == f && b == f) {
      return Hue::MAGENTA;
    } else if (r == f) {
      return Hue::RED;
    } else if (g == f) {
      return Hue::GREEN;
    } else if (b == f) {
      return Hue::BLUE;
    } else if (b == z) {
      return Hue::YELLOW;
    } else if (r == z) {
      return Hue::CYAN;
    } else if (g == z) {
      return Hue::MAGENTA;
    }
  }
  return Hue::UNKNOWN;
}

Brightness what_brightness(const Pixel &pixel) {
  const byte r = pixel.red, g = pixel.green, b = pixel.blue;
  const byte c = 0xC0;
  const auto mini = std::min(r, std::min(g, b));
  const auto maxi = std::max(r, std::max(g, b));
  if (mini == 0xFF) {
    return Brightness::WHITE;
  } else if (mini == c) {
    return Brightness::LIGHT;
  } else if (maxi == c) {
    return Brightness::DARK;
  } else if (maxi == 0x00) {
    return Brightness::BLACK;
  } else {
    return Brightness::NORMAL;
  }
}

Color what_color(const Pixel &pixel) {
  Color color = {what_hue(pixel), what_brightness(pixel)};
  return color;
}

CodelsTable normalize(const Image &image, const std::size_t codel_size) {
  //if (codel_size == 0) codel_size = detect_codel_size(image);
  const std::size_t height = image.get_height();
  const std::size_t width = image.get_width();
  CodelsTable codels_table(height, width);
  for (std::size_t i = 0; i < height; ++i) {
    for (std::size_t j = 0; j < width; ++j) {
      codels_table.set(i, j, what_color(image[i*codel_size][j*codel_size]));
    }
  }
  return codels_table;
}

// template <typename T>
// using Table = std::vector<std::vector<T>>;
// 
// template <typename T>
// Table<T> make_table(const std::size_t height, const std::size_t width, const T& val) {
//   return Table<T>(height, std::vector<T>(width, T));
// }
// 
// void fill(const CodelsTable &table, Table<int64_t> colorblock_number,
//     int64_t i, int64_t j) {
//   int64_t height = table.height();
//   int64_t width = table.width();
//   int64_t d[] = {1, 0, -1, 0, 1}; // cos(90n degree)
//   for (int k = 0; k < 4; ++k) {
//     int64_t ni = i+d[k+1];
//     int64_t nj = j+d[k];
//     if (ni < 0 || ni >= height || nj < 0 || nj >= width) continue;
//     if (table[i][j] == table[ni][nj]) {
//       if (colorblock_number[ni][nj] == -1) {
//         colorblock_number[ni][nj] = colorblock_number[i][j];
//         fill(table, colorblock_number, ni, nj, colorblock_n, rect);
//       }
//     }
//   }
// }

int diff_hue(const Hue &now, const Hue &next) {
  int now_i = static_cast<int>(now);
  int next_i = static_cast<int>(next);
  return (next_i - now_i) + (next_i >= now_i ? 0 : 6);
}

int diff_brightness(const Brightness &now, const Brightness &next) {
  int now_i = static_cast<int>(now);
  int next_i = static_cast<int>(next);
  return (next_i - now_i) + (next_i >= now_i ? 0 : 3);
}

CommandType colorDiff2CommandType(const Color &now, const Color &next) {
  if (is_black_or_white(now) || is_black_or_white(next)) return CommandType::NOP;
  int huediff = diff_hue(now.hue, next.hue);
  int brightnessdiff = diff_brightness(now.brightness, next.brightness);
  return static_cast<CommandType>(huediff * 3 + brightnessdiff);
}

CommandType color_to_command(const Color from, const Color to) {
  return colorDiff2CommandType(from, to);
}

// using Pos = std::tuple<int64_t, int64_t>;
// 
// struct ColorBlock {
//   Rectangle rect;
//   int64_t size;
//   Color color;
//   bool halt;
//   std::array<std::array<int64_t, 2>, 4> next;
//   ColorBlock(cosnt Rectangle &rect, const int64_t size, const Color &color, bool halt = false)
//     : rect(rect), size(size), color(color) {}
// };
// 
// std::tuple<Table<int64_t>, std::vector<ColorBlock>, int64_t>
//     fill_colorblocks(const CodelsTable &table) {
//   int64_t height = table.height();
//   int64_t width = table.width();
//   auto colorblock_number = make_table<int64_t>(height, width, -1);
//   int64_t colorblock_count = 0;
//   std::vector<ColorBlock> colorblocks;
//   for (int64_t i = 0; i < height; ++i) {
//     for (int64_t j = 0; j < width; ++j) {
//       if (colorblock_number[i][j] == -1) {
//         if (is_color(table[i][j])) {
//           colorblock_number[i][j] = colorblock_count++;
//           fill(table, colorblock_number, i, j);
//         }
//       }
//       if (colorblock_number[i][j] >= 0) {
//         auto& blockref = colorblocks[colorblock_number[i][j]];
//         blockref.rect.update(i, j);
//         ++blockref.size;
//         blockref.color = table[i][j];
//       }
//     }
//   }
//   return std::make_tuple(colorblock_number, colorblock_count);
// }
// 
// std::vector<Command> parse(const CodelsTable &table) {
//   int64_t height = table.height();
//   int64_t width = table.width();
//   Table<int64_t> colorblock_number;
//   int64_t colorblock_count;
//   std::vector<ColorBlock> colorblocks;
//   std::tie(colorblock_number, colorblocks, colorblock_count) = fill_colorblocks(table);
//   std::vector<Command> commands;
//   for (const auto &colorblock : colorblocks) {
//     for (int k = 0; k < 4; ++k) {
//       for (int l = 0; l < 2; ++l) {
//         
//       }
//     }
//   }
// }
// 
// int64_t parse_impl(const CodelsTable &table, const int64_t i, const int64_t j) {
//   if (colorblock_number[i][j] >= 0) continue;
//   colorblock_number[i][j] = colorblocks.size();
//   Rectangle rect(i, j);
//   int64_t colorblock_size = fill(table, colorblock_number[i][j], i, j, rect);
//   colorblocks.emplace_back(rect, colorblock_size, table[i][j]);
//   bool is_valid[4][2] = {{}};
//   bool halt = true;
//   Pos next[4][2];
//   int64_t d[] = {-1, 1, 1, -1};
//   int64_t size[] = {height, width};
//   for (int k = 0; k < 4; ++k) {
//     for (int l = 0; l < 2; ++l) {
//       int64_t np = rect.cc(k).get(l);
//       for (int64_t nq = rect.dp(k)-1; nq >= 0 && nq < size[k%2]; nq += d[k]) {
//         int64_t ni = k%2 ? np : nq;
//         int64_t nj = k%2 ? nq : np;
//         if (!is_black_or_white(table[ni][nj])) {
//           halt = false;
//           colorblocks[colorblock_number[i][j]].next[k][l] = parse_impl(ni, nj);
//           next[k][l] = Pos(ni, nj);
//           break;
//         }
//       }
//     }
//   }
//   if (!halt) {
//     for (int k = 0; k < 4; ++k) {
//       for (int l = 0; l < 2; ++l) {
//         int nl = l;
//         for (int m = k; m < 4+k; ++m) {
//           if (is_valid[m%4][nl]) {
//             next[k][l] = next[m%4][nl];
//             colorblocks[1];
//             break;
//           }
//           nl = 1-nl;
//           if (is_valid[m%4][nl]) {
//             next[k][l] = next[m%4][nl];
//             break;
//           }
//         }
//       }
//     }
//   }
// }
