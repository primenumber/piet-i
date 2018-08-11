#pragma once
#include <cassert>
#include <array>
#include <set>
#include <tuple>
#include <vector>
#include <omp.h>
#include "codel.hpp"
#include "fillmap.hpp"

class Range {
 public:
  Range(const size_t min, const size_t max)
    : min(min), max(max) {}
  void update(const size_t x) {
    min = std::min(min, x);
    max = std::max(max, x);
  }
  void reset(const size_t x) {
    min = max = x;
  }
  size_t min;
  size_t max;
};

class Bound {
 public:
  size_t top, bottom, left, right;
  Range top_r, bottom_r, left_r, right_r;
  Bound(size_t width, size_t height)
    : top(height-1), bottom(0), left(width-1), right(0),
      top_r(width-1, 0), bottom_r(width-1, 0),
      left_r(height-1, 0), right_r(height-1, 0) {}
  void update(size_t x, size_t y) {
    if (x > right) {
      right = x;
      right_r.reset(y);
    } else if (x == right) {
      right_r.update(y);
    }
    if (x < left) {
      left= x;
      left_r.reset(y);
    } else if (x == left) {
      left_r.update(y);
    }
    if (y > bottom) {
      bottom = y;
      bottom_r.reset(x);
    } else if (y == bottom) {
      bottom_r.update(x);
    }
    if (y < top) {
      top = y;
      top_r.reset(x);
    } else if (y == top) {
      top_r.update(x);
    }
  }
};

class ColorBlock {
 public:
  using index_t = std::tuple<int32_t, uint8_t, uint8_t, bool, bool>;
  ColorBlock(const Color color, const size_t size)
    : color_(color), size_(size) {
    next_blocks.fill(std::make_tuple(-1, 0, 0, false, false));
  }
  void set_next_block(index_t index, uint8_t dp, uint8_t cc);
  index_t get_next_block(uint8_t dp, uint8_t cc) const;
  Color get_color() const { return color_; }
  size_t get_size() const { return size_; }
 private:
  Color color_;
  size_t size_;
  std::array<index_t, 8> next_blocks;
};

template <typename T>
using cache_line_t = std::vector<std::array<std::array<T, 2>, 4>>;

template <typename T>
using cache_t = std::vector<cache_line_t<T>>;

ColorBlock::index_t search(size_t width, size_t height, int64_t x, int64_t y, uint8_t dp, uint8_t cc, const FillMap &fm, cache_t<ColorBlock::index_t> &cache, cache_t<bool> &visit);

std::tuple<std::vector<Bound>, std::vector<int32_t>> get_bounds(
    const FillMap &fill_map, const size_t width, const size_t height);

template <typename T>
cache_t<T> make_cache_buf(const size_t width, const size_t height, const T& elem) {
  std::array<std::array<T, 2>, 4> ary = {{
    {{elem, elem}},
    {{elem, elem}},
    {{elem, elem}},
    {{elem, elem}}
  }};
  cache_t<T> cache(height);
#pragma omp parallel for
  for (size_t i = 0; i < height; ++i) {
    cache[i].resize(width, ary);
  }
  return cache;
}

class ColorBlockGraph {
 public:
  template <typename CodelTable>
  explicit ColorBlockGraph(const CodelTable &table) {
    const size_t height = table.height();
    const size_t width = table.width();
    FillMap fill_map(width, height, table);
    fill_map.fill_all();
    auto [bounds, count] = get_bounds(fill_map, width, height);
    blocks.reserve(bounds.size());
    for (size_t i = 0; i < bounds.size(); ++i) {
      const auto &bound = bounds[i];
      size_t x = bound.top_r.min;
      size_t y = bound.top;
      blocks.emplace_back(table[y][x], count[i]);
    }
    ColorBlock::index_t invalid(-1, 0, 0, false, false);
    auto cache = make_cache_buf<ColorBlock::index_t>(width, height, invalid);
    std::vector<cache_t<bool>> visits(omp_get_max_threads());
#pragma omp parallel for
    for (size_t i = 0; i < omp_get_max_threads(); ++i) {
      visits[i] = cache_t<bool>(height, cache_line_t<bool>(width, {{}}));
    }
#pragma omp parallel for
    for (size_t i = 0; i < bounds.size(); ++i) {
      cache_t<bool> &visit = visits[omp_get_thread_num()];
      const auto &bound = bounds[i];
      size_t x = bound.top_r.min;
      size_t y = bound.top;
      auto next = search(width, height, x, y, 3, 0, fill_map, cache, visit);
      blocks[i].set_next_block(next, 3, 0);
      x = bound.top_r.max;
      next = search(width, height, x, y, 3, 1, fill_map, cache, visit);
      blocks[i].set_next_block(next, 3, 1);
      x = bound.right;
      y = bound.right_r.min;
      next = search(width, height, x, y, 0, 0, fill_map, cache, visit);
      blocks[i].set_next_block(next, 0, 0);
      y = bound.right_r.max;
      next = search(width, height, x, y, 0, 1, fill_map, cache, visit);
      blocks[i].set_next_block(next, 0, 1);
      x = bound.bottom_r.max;
      y = bound.bottom;
      next = search(width, height, x, y, 1, 0, fill_map, cache, visit);
      blocks[i].set_next_block(next, 1, 0);
      x = bound.bottom_r.min;
      next = search(width, height, x, y, 1, 1, fill_map, cache, visit);
      blocks[i].set_next_block(next, 1, 1);
      x = bound.left;
      y = bound.left_r.max;
      next = search(width, height, x, y, 2, 0, fill_map, cache, visit);
      blocks[i].set_next_block(next, 2, 0);
      y = bound.left_r.min;
      next = search(width, height, x, y, 2, 1, fill_map, cache, visit);
      blocks[i].set_next_block(next, 2, 1);
    }
  }
  size_t size() const { return blocks.size(); }
  ColorBlock &operator[](const size_t index) { return blocks[index]; }
  const ColorBlock &operator[](const size_t index) const { return blocks[index]; }
 private:
  std::vector<ColorBlock> blocks;
};
