#include "stack.hpp"
#include <iostream>
#include <locale>
#include <codecvt>

int32_t get_number() {
  int32_t value;
  std::cin >> value;
  return value;
}

constexpr int32_t eof = std::char_traits<char32_t>::eof();

int32_t get_char() {
  char head;
  if(!std::cin.get(head)) return eof;
  unsigned char uhead = head;
  if (uhead <= 0x7F) {
    return head;
  } else {
    int length;
    int32_t res = 0;
    if (0xC2 <= uhead && uhead <= 0xDF) {
      length = 1;
    } else if (0xE0 <= uhead && uhead <= 0xEF) {
      length = 2;
    } else if (0xF0 <= uhead && uhead <= 0xF7) {
      length = 3;
    } else {
      std::cin.unget();
      return eof;
    }
    res |= (static_cast<int32_t>(uhead) & ~(0xFF << (6 - length))) << (length * 6);
    for(--length; length >= 0; --length) {
      char tail;
      if (!std::cin.get(tail)) return eof;
      res |= (static_cast<int32_t>(tail) & 0x3F) << (length * 6);
    }
    return res;
  } 
}

void Stack::roll(const int32_t depth, const int32_t iter) {
  std::vector<int32_t> buf(depth);
  for (int i = 0; i < iter; ++i) {
    buf[i] = data[data.size() - iter + i];
  }
  for (int i = 0; i < depth - iter; ++i) {
    buf[i+iter] = data[data.size() - depth + i];
  }
  for (int i = 0; i < depth; ++i) {
    data[data.size() - depth + i] = buf[i];
  }
}

int32_t mod(int32_t x, int32_t d) {
  auto y = x % d;
  if (y < 0) y += d;
  return y;
}

void Stack::roll() {
  if (size() >= 2) {
    int iter = top(); pop();
    int depth = top(); pop();
    if (depth >= 0 && size() >= (size_t)depth) {
      if (depth > 0) {
        roll(depth, ::mod(iter, depth));
      }
    } else {
      push(depth);
      push(iter);
    }
  } 
}

int32_t Stack::switch_() {
  if (!empty()) {
    int32_t value;
    value = top();
    pop();
    return ::mod(value, 2);
  } else {
    return 0;
  }
}

int32_t Stack::pointer() {
  if (!empty()) {
    int32_t value;
    value = top();
    pop();
    return ::mod(value, 4);
  } else {
    return 0;
  }
}

int32_t Stack::eq_zero() {
  if (!empty()) {
    int32_t value;
    value = top();
    pop();
    if (value == 0) {
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

void putchar_32(const int32_t val) {
  char32_t ch = static_cast<char32_t>(val);
  std::u32string u32s(1, ch);
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> u32tou8;
  std::cout << u32tou8.to_bytes(u32s);
}

//std::shared_ptr<Command> InNumber::exec(Stack & stack) const {
//  //std::cerr << "InNumber" << std::endl;
//  int32_t value;
//  std::cin >> value;
//  stack.push(value);
//  return next.lock();
//}
//
//std::shared_ptr<Command> InChar::exec(Stack & stack) const {
//  //std::cerr << "InChar" << std::endl;
//  stack.push(io32::getchar());
//  return next.lock();
//}
//
//std::shared_ptr<Command> Pop::exec(Stack & stack) const {
//  //std::cerr << "Pop" << std::endl;
//  if (!stack.empty()) stack.pop();
//  return next.lock();
//}

void Stack::out_number() {
  if (!empty()) {
    std::cout << top();
    pop();
  }
}

void Stack::out_char() {
  if (!empty()) {
    putchar_32(top());
    pop();
  }
}

template <typename Func>
void Stack::bin_op(Func func) noexcept {
  if (size() >= 2) {
    int arg2 = top(); pop();
    int arg1 = top(); pop();
    try {
      int result = func(arg1, arg2);
      push(result);
    } catch (...) {
      push(arg1);
      push(arg2);
    }
  } 
}

void Stack::add() {
  bin_op(std::plus<int32_t>());
}

void Stack::sub() {
  bin_op(std::minus<int32_t>());
}

void Stack::mul() {
  bin_op(std::multiplies<int32_t>());
}

void Stack::div() {
  bin_op(std::divides<int32_t>());
}

void Stack::mod() {
  bin_op(std::modulus<int32_t>());
}

void Stack::greater() {
  bin_op(std::greater<int32_t>());
}

void Stack::not_() {
  if (!empty()) {
    int32_t value = top();
    pop();
    if (value) {
      push(0);
    } else {
      push(1);
    }
  }
}

void Stack::duplicate() {
  if (!empty()) {
    int32_t value = top();
    push(value);
  }
}
