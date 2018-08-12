#pragma once
#include <cstdint>
#include <vector>

int32_t get_number();
int32_t get_char();

class Stack {
 public:
  Stack() : data() {}
  int32_t top() const { return data.back(); }
  bool empty() const noexcept { return data.empty(); }
  std::size_t size() const noexcept { return data.size(); }
  void pop() {
    if (!empty()) {
      data.pop_back();
    }
  }
  void push(const int32_t x) { data.push_back(x); }
  void push_array(const std::vector<int32_t> &ary) {
    using std::begin;
    using std::end;
    data.insert(end(data), begin(ary), end(ary));
  }
  void roll(const int32_t depth, const int32_t iter);
  void roll();
  void add();
  void sub();
  void mul();
  void div();
  void mod();
  void greater();
  void duplicate();
  void not_();
  void out_number();
  void out_char();
  int32_t switch_();
  int32_t pointer();
  int32_t eq_zero();
template <typename Func>
  void bin_op(Func func) noexcept;
 private:
  std::vector<int32_t> data;
};
