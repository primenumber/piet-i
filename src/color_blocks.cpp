#include "color_blocks.hpp"

ColorBlock::index_t search(size_t width, size_t height, int64_t x, int64_t y, size_t dp, size_t cc, const FillMap &fm, std::map<std::tuple<size_t, size_t, size_t, size_t>, ColorBlock::index_t> &cache) {
  int dx[] = {1, 0, -1, 0};
  int dy[] = {0, 1, 0, -1};
  std::set<std::tuple<size_t, size_t, size_t, size_t>> s;
  bool first = true;
  while (true) {
    auto itr = cache.find(std::make_tuple(x, y, dp, cc));
    if (itr != std::end(cache)) {
      for (auto t : s) {
        cache[t] = itr->second;
      }
      return itr->second;
    }
    if (s.find(std::make_tuple(x, y, dp, cc)) != std::end(s)) {
      ColorBlock::index_t res(-1, 0, 0, true);
      for (auto t : s) {
        cache[t] = res;
      }
      return res;
    }
    s.emplace(x, y, dp, cc);
    int nx = x + dx[dp];
    int ny = y + dy[dp];
    if (nx < 0 || ny < 0 || nx >= width || ny >= height) {
      if (fm.is_white(x, y)) {
        cc = 1 - cc;
        dp++;
        dp %= 4;
      } else {
        ColorBlock::index_t res(-1, 0, 0, false);
        for (auto t : s) {
          cache[t] = res;
        }
        return res;
      }
    } else if (fm.is_black(nx, ny)) {
      if (fm.is_white(x, y)) {
        cc = 1 - cc;
        dp++;
        dp %= 4;
      } else {
        ColorBlock::index_t res(-1, 0, 0, false);
        for (auto t : s) {
          cache[t] = res;
        }
        return res;
      }
    } else if (fm.is_color(nx, ny)) {
      ColorBlock::index_t res(fm.get_index(nx, ny), dp, cc, first);
      for (auto t : s) {
        cache[t] = res;
      }
      return res;
    } else {
      x = nx;
      y = ny;
    }
    first = false;
  }
}
