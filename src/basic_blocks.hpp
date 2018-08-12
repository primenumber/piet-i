#include <array>
#include <iostream>
#include <vector>
#include "interpret.hpp"

class BasicBlock {
 public:
  BasicBlock() = default;
  void push(const std::shared_ptr<Command> &cmd) {
    commands.push_back(cmd);
  }
  void set_nexts(const std::vector<int32_t> &nexts) {
    next_index = nexts;
  }
  int32_t exec(Stack &) const;
  friend std::ostream& operator<<(std::ostream &os, const BasicBlock &bb);
 private:
  std::vector<std::shared_ptr<Command>> commands;
  std::vector<int32_t> next_index;
};

class BasicBlockGraph {
 public:
  explicit BasicBlockGraph(const CommandGraph &cg);
  void exec() const;
  friend std::ostream& operator<<(std::ostream &os, const BasicBlockGraph &bbg);
 private:
  std::vector<BasicBlock> basic_blocks;
};
