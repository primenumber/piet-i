#pragma once
#include "utils.hpp"

enum class CommandType {
  NOP,
  PUSH,
  POP,
  ADD,
  SUBTRACT,
  MULTIPLY,
  DIVIDE,
  MOD,
  NOT,
  GREATER,
  POINTER,
  SWITCH,
  DUPLICATE,
  ROLL,
  INN,
  INC,
  OUTN,
  OUTC,
  HALT
};

CodelsTable normalize(const Image &image, const std::size_t codel_size = 0);
CommandType color_to_command(const Color from, const Color to);
