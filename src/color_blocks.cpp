#include "color_blocks.hpp"

void ColorBlock::set_next_block(ColorBlock::index_t index, uint8_t dp, uint8_t cc) {
  assert(dp < 4);
  assert(cc < 2);
  next_blocks[dp * 2 + cc] = index;
}

ColorBlock::index_t ColorBlock::get_next_block(uint8_t dp, uint8_t cc) const {
  assert(dp < 4);
  assert(cc < 2);
  return next_blocks[dp * 2 + cc];
}

std::tuple<std::vector<Bound>, std::vector<int32_t>> get_bounds(
    const FillMap &fill_map, const size_t width, const size_t height) {
  std::vector<Bound> bounds(fill_map.index_count(), Bound(width, height));
  std::vector<int32_t> count(fill_map.index_count(), 0);
  for (size_t i = 0; i < height; ++i) {
    for (size_t j = 0; j < width; ++j) {
      if (fill_map.get_index(j, i) >= 0) {
        bounds[fill_map.get_index(j, i)].update(j, i);
        ++count[fill_map.get_index(j, i)];
      }
    }
  }
  return std::make_tuple(std::move(bounds), std::move(count));
}

// Lockless write
// x86_64 guarantee 8byte aligned 8byte write atomically
void write_cache(
    cache_t<ColorBlock::index_t> &cache,
    std::vector<std::tuple<size_t, size_t, uint8_t, uint8_t>> &&s,
    const ColorBlock::index_t &res) {
  for (auto &&t : s) {
    auto [x, y, dp, cc] = t;
    cache[y][x][dp][cc] = res;
  }
}

ColorBlock::index_t search(size_t width, size_t height,
    int64_t x, int64_t y, uint8_t dp, uint8_t cc, const FillMap &fm,
    cache_t<ColorBlock::index_t> &cache,
    cache_t<bool> &visit) {
  int dx[] = {1, 0, -1, 0};
  int dy[] = {0, 1, 0, -1};
  std::vector<std::tuple<size_t, size_t, uint8_t, uint8_t>> s;
  s.reserve(8);
  bool first = true;
  while (true) {
    auto elem = cache[y][x][dp][cc];
    if (std::get<4>(elem)) {
      write_cache(cache, std::move(s), elem);
      return elem;
    }
    if (visit[y][x][dp][cc]) {
      ColorBlock::index_t res(-1, 0, 0, true, true);
      write_cache(cache, std::move(s), res);
      return res;
    }
    s.emplace_back(x, y, dp, cc);
    visit[y][x][dp][cc] = true;
    int64_t nx = x + dx[dp];
    int64_t ny = y + dy[dp];
    if (nx < 0 || ny < 0 || nx >= (int64_t)width || ny >= (int64_t)height
        || fm.is_black(nx, ny)) {
      if (fm.is_white(x, y)) {
        cc = 1 - cc;
        dp++;
        dp %= 4;
      } else {
        ColorBlock::index_t res(-1, 0, 0, false, true);
        write_cache(cache, std::move(s), res);
        return res;
      }
    } else if (fm.is_color(nx, ny)) {
      ColorBlock::index_t res(fm.get_index(nx, ny), dp, cc, first, true);
      write_cache(cache, std::move(s), res);
      return res;
    } else {
      x = nx;
      y = ny;
    }
    first = false;
  }
}
