#pragma once
#include <vector>
#include "codel.hpp"
#include "utils.hpp"

class FillMap {
 public:
  using index_t = int32_t;
  FillMap(const size_t width, const size_t height, const CodelTable& ref_table)
    : width_(width), height_(height), index(0),
      index_table(height, std::vector<index_t>(width, -1)),
      visited_table(height, std::vector<bool>(width, false)),
      filled_size(),
      ref_table_(ref_table){}
  index_t fill_all();
  size_t fill(const size_t x, const size_t y) {
    size_t res = fill_impl(x, y);
    ++index;
    return res;
  }
  index_t get_index(const size_t x, const size_t y) const {
    return index_table[y][x];
  }
  bool is_black(const size_t x, const size_t y) const {
    return ::is_black(ref_table_[y][x]);
  }
  bool is_white(const size_t x, const size_t y) const {
    return ::is_white(ref_table_[y][x]);
  }
  bool is_color(const size_t x, const size_t y) const {
    return ::is_color(ref_table_[y][x]);
  }
  index_t index_count() const { return index; }
 private:
  size_t fill_impl(const int x, const int y);
  const size_t width_;
  const size_t height_;
  index_t index;
  std::vector<std::vector<index_t>> index_table;
  std::vector<std::vector<bool>> visited_table;
  std::vector<size_t> filled_size;
  const CodelTable & ref_table_;
};
