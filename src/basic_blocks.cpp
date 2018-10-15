#include "basic_blocks.hpp"
#include <algorithm>
#include <memory>
#include <map>
#include <queue>
#include <set>

int32_t BasicBlock::exec(Stack &stack) const {
  using std::begin;
  using std::end;
  for (size_t i = 0; i < commands.size(); ++i) {
    auto res = commands[i]->exec(stack);
    if (i == commands.size() - 1) {
      if (res == nullptr) return -1;
      auto nexts = commands[i]->get_nexts();
      auto itr = std::find(begin(nexts), end(nexts), res);
      return next_index[itr - begin(nexts)];
    }
  }
  return -1;
}

std::ostream& operator<<(std::ostream &os, const BasicBlock &bb) {
  for (size_t i = 0; i < bb.commands.size(); ++i) {
    os << bb.commands[i]->to_cpp_string();
    if (i == bb.commands.size() - 1) {
      size_t length = bb.next_index.size();
      for (size_t j = 0; j < length; ++j) {
        if (j != length - 1) {
          os << "    case " << j << ":\n";
        } else if (j) {
          os << "    default:\n";
        }
        os << "      goto label" << bb.next_index[j] << ";\n";
      }
      if (length == 0) {
        os << "  exit(0);\n";
      }
      if (length <= 1) break;
      os << "  }\n";
    }
  }
  return os;
}

BasicBlockGraph::BasicBlockGraph(const CommandGraph &cg) {
  using std::end;
  std::map<std::shared_ptr<Command>, int32_t> ptr_to_index;
  ptr_to_index[cg.root()] = 0;
  std::queue<std::shared_ptr<Command>> q;
  q.push(cg.root());
  std::vector<int32_t> push_stack;
  int32_t pop_count = 0;
  while (!q.empty()) {
    auto ptr = q.front();
    std::shared_ptr<Command> old_ptr;
    q.pop();
    basic_blocks.emplace_back();
    std::set<std::shared_ptr<Command>> s;
    while (true) {
      if (s.count(ptr)) {
        auto itr = ptr_to_index.find(ptr);
        std::vector<int32_t> next_index;
        if (itr != end(ptr_to_index)) {
          next_index.push_back(itr->second);
        } else {
          const int32_t index = ptr_to_index.size();
          next_index.push_back(index);
          ptr_to_index[ptr] = index;
          q.push(ptr);
        }
        if (push_stack.size() > 1) {
          basic_blocks.back().push(std::make_shared<PushArray>(push_stack));
        } else if (push_stack.size() == 1) {
          basic_blocks.back().push(old_ptr);
        }
        if (pop_count > 1) {
          basic_blocks.back().push(std::make_shared<Pop>(pop_count));
        } else if (pop_count == 1) {
          basic_blocks.back().push(old_ptr);
        }
        push_stack.clear();
        pop_count = 0;
        break;
      }
      s.insert(ptr);
      ConcreteCommandType type = ptr->command_type();
      bool lazy = false;
      if (type == ConcreteCommandType::Push) {
        push_stack.push_back(dynamic_cast<Push*>(ptr.get())->get_value());
        lazy = true;
      } else {
        if (push_stack.size() > 1) {
          basic_blocks.back().push(std::make_shared<PushArray>(push_stack));
        } else if (push_stack.size() == 1) {
          basic_blocks.back().push(old_ptr);
        }
        push_stack.clear();
      }
      if (type == ConcreteCommandType::Pop) {
        ++pop_count;
        lazy = true;
      } else {
        if (pop_count > 1) {
          basic_blocks.back().push(std::make_shared<Pop>(pop_count));
        } else if (pop_count == 1) {
          basic_blocks.back().push(old_ptr);
        }
        pop_count = 0;
      }
      if (!lazy) {
        basic_blocks.back().push(ptr);
      }
      auto nexts = ptr->get_nexts();
      if (nexts.size() != 1) {
        std::vector<int32_t> next_index;
        for (auto next : nexts) {
          auto itr = ptr_to_index.find(next);
          if (itr != end(ptr_to_index)) {
            next_index.push_back(itr->second);
          } else {
            const int32_t index = ptr_to_index.size();
            next_index.push_back(index);
            ptr_to_index[next] = index;
            q.push(next);
          }
        }
        basic_blocks.back().set_nexts(next_index);
        break;
      } else {
        old_ptr = ptr;
        ptr = nexts.front();
      }
    }
  }
}

void BasicBlockGraph::exec() const {
  int32_t index = 0;
  Stack stack;
  while (index >= 0) {
    index = basic_blocks[index].exec(stack);
  }
}

std::ostream& operator<<(std::ostream &os, const BasicBlockGraph &bbg) {
  os << "#include <cstdlib>\n";
  os << "#include \"lib/stack.hpp\"\n";
  os << "int main() {\n";
  os << "  Stack stack;\n";
  for (size_t i = 0; i < bbg.basic_blocks.size(); ++i) {
    os << "  label" << i << ":\n" << bbg.basic_blocks[i] << "\n";
  }
  os << "}";
  return os;
}
