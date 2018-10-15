#include "io32.hpp"
#include <locale>
#include <codecvt>
#include <iostream>

namespace io32 {

constexpr int_type eof = std::char_traits<char32_t>::eof();

int_type getchar() {
  char head;
  if(!std::cin.get(head)) return eof;
  unsigned char uhead = head;
  if (uhead <= 0x7F) {
    return head;
  } else {
    int length;
    int_type res = 0;
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
    res |= (static_cast<int_type>(uhead) & ~(0xFF << (6 - length))) << (length * 6);
    for(--length; length >= 0; --length) {
      char tail;
      if (!std::cin.get(tail)) return eof;
      res |= (static_cast<int_type>(tail) & 0x3F) << (length * 6);
    }
    return res;
  } 
}

void putchar(const int_type val) {
  char32_t ch = static_cast<char32_t>(val);
  std::u32string u32s(1, ch);
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> u32tou8;
  std::cout << u32tou8.to_bytes(u32s);
}

} // namespace io32
