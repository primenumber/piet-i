#include "fillmap.hpp"
#include <queue>
#include <tuple>

FillMap::index_t FillMap::fill_all() {
  for (size_t i = 0; i < height_; ++i) {
    for (size_t j = 0; j < width_; ++j) {
      if (!visited_table[i][j] && ::is_color(ref_table_[i][j])) {
        filled_size.push_back(fill(j, i));
      }
    }
  }
  return index;
}

size_t FillMap::fill_impl(const int x, const int y) {
  size_t count = 0;
  visited_table[y][x] = true;
  std::queue<std::tuple<int64_t, int64_t>> q;
  q.emplace(x, y);
  while (!q.empty()) {
    auto [qx, qy] = q.front();
    q.pop();
    index_table[qy][qx] = index;
    ++count;
    const int dx[] = {0, -1, 0, 1};
    const int dy[] = {1, 0, -1, 0};
    for (int i = 0; i < 4; ++i) {
      const int64_t nx = qx + dx[i];
      const int64_t ny = qy + dy[i];
      if (nx < 0 || nx >= width_ || ny < 0 || ny >= height_) continue;
      if (visited_table[ny][nx]) continue;
      if (ref_table_[qy][qx] != ref_table_[ny][nx]) continue;
      visited_table[ny][nx] = true;
      q.emplace(nx, ny);
    }
  }
  return count;
}
