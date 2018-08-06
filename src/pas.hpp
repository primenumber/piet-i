#include <vector>
#include <string>

namespace pas {

enum class PASCommandType {
  PUSH,
  POP,
  ADD,
  SUB,
  MUL,
  DIV,
  MOD,
  NOT,
  GREATER,
  DUP,
  ROLL,
  INN,
  INC,
  OUTN,
  OUTC,
  HALT,
  LABEL,
  JEZ,
  JMP,
  SWAP,
  UNKNOWN
};

struct PASCommand {
  PASCommandType inst;
  int arg1;
  int arg2;
  PASCommand(const PASCommandType inst) : inst(inst) {}
  PASCommand(const PASCommandType inst, const int arg1) : inst(inst), arg1(arg1) {}
  PASCommand(const PASCommandType inst, const int arg1, const int arg2)
    : inst(inst), arg1(arg1), arg2(arg2) {}
};

class PAS {
 public:
  explicit PAS(const std::vector<std::string> &);
  std::vector<PASCommand> commands;
};

} // namespace pas
