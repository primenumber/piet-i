#pragma once
#include <cassert>
#include <array>
#include <map>
#include <set>
#include <tuple>
#include <vector>
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
  using index_t = std::tuple<int32_t, size_t, size_t, bool>;
  ColorBlock(const Color color, const size_t size)
    : color_(color), size_(size) {
    next_blocks.fill(std::make_tuple(-1, 0, 0, false));
  }
  void set_next_block(index_t index, size_t dp, size_t cc) {
    assert(dp < 4);
    assert(cc < 2);
    next_blocks[dp * 2 + cc] = index;
  }
  index_t get_next_block(size_t dp, size_t cc) const {
    assert(dp < 4);
    assert(cc < 2);
    return next_blocks[dp * 2 + cc];
  }
  Color get_color() const { return color_; }
  size_t get_size() const { return size_; }
 private:
  Color color_;
  size_t size_;
  std::array<index_t, 8> next_blocks;
};

ColorBlock::index_t search(size_t width, size_t height, int64_t x, int64_t y, size_t dp, size_t cc, const FillMap &fm, std::map<std::tuple<size_t, size_t, size_t, size_t>, ColorBlock::index_t> &cache);

class ColorBlockGraph {
 public:
  template <typename CodelTable>
  explicit ColorBlockGraph(const CodelTable &table) {
    const size_t height = table.height();
    const size_t width = table.width();
    FillMap fill_map(width, height, table);
    fill_map.fill_all();
    std::vector<Bound> bounds(fill_map.index_count(), Bound(width, height));
    std::vector<int> count(fill_map.index_count(), 0);
    for (size_t i = 0; i < height; ++i) {
      for (size_t j = 0; j < width; ++j) {
        if (fill_map.get_index(j, i) >= 0) {
          bounds[fill_map.get_index(j, i)].update(j, i);
          ++count[fill_map.get_index(j, i)];
        }
      }
    }
    std::map<std::tuple<size_t, size_t, size_t, size_t>, ColorBlock::index_t> cache;
    for (size_t i = 0; i < bounds.size(); ++i) {
      const auto &bound = bounds[i];
      size_t x = bound.top_r.min;
      size_t y = bound.top;
      blocks.emplace_back(table[y][x], count[i]);
      auto next = search(width, height, x, y, 3, 0, fill_map, cache);
      blocks[i].set_next_block(next, 3, 0);
      x = bound.top_r.max;
      next = search(width, height, x, y, 3, 1, fill_map, cache);
      blocks[i].set_next_block(next, 3, 1);
      x = bound.right;
      y = bound.right_r.min;
      next = search(width, height, x, y, 0, 0, fill_map, cache);
      blocks[i].set_next_block(next, 0, 0);
      y = bound.right_r.max;
      next = search(width, height, x, y, 0, 1, fill_map, cache);
      blocks[i].set_next_block(next, 0, 1);
      x = bound.bottom_r.max;
      y = bound.bottom;
      next = search(width, height, x, y, 1, 0, fill_map, cache);
      blocks[i].set_next_block(next, 1, 0);
      x = bound.bottom_r.min;
      next = search(width, height, x, y, 1, 1, fill_map, cache);
      blocks[i].set_next_block(next, 1, 1);
      x = bound.left;
      y = bound.left_r.max;
      next = search(width, height, x, y, 2, 0, fill_map, cache);
      blocks[i].set_next_block(next, 2, 0);
      y = bound.left_r.min;
      next = search(width, height, x, y, 2, 1, fill_map, cache);
      blocks[i].set_next_block(next, 2, 1);
    }
  }
  size_t size() const { return blocks.size(); }
  ColorBlock &operator[](const size_t index) { return blocks[index]; }
  const ColorBlock &operator[](const size_t index) const { return blocks[index]; }
 private:
  std::vector<ColorBlock> blocks;
};
