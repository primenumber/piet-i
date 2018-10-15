#pragma once
#include <memory>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <vector>
#include "pas.hpp"
#include "color_blocks.hpp"

class Stack {
 public:
  Stack() : data() {}
  int32_t top() const { return data.back(); }
  bool empty() const { return data.empty(); }
  std::size_t size() const { return data.size(); }
  void pop() { data.pop_back(); }
  void push(const int32_t x) { data.push_back(x); }
  void push_array(const std::vector<int32_t> &ary) {
    using std::begin;
    using std::end;
    data.insert(end(data), begin(ary), end(ary));
  }
  void roll(const int32_t depth, const int32_t iter);
  std::string to_s() const {
    std::stringstream ss;
    for (int32_t v : data) {
      ss << " " << v;
    }
    return ss.str();
  }
 private:
  std::vector<int32_t> data;
};

enum class ConcreteCommandType {
  Switch,
  Pointer,
  Jez,
  Halt,
  Nop,
  Push,
  PushArray,
  Duplicate,
  InNumber,
  InChar,
  Pop,
  OutNumber,
  OutChar,
  Add,
  Subtract,
  Multiply,
  Divide,
  Modulo,
  Greater,
  Not,
  Swap,
  Roll
};

class Command {
 public:
  virtual std::shared_ptr<Command> exec(Stack &) const = 0;
  virtual std::vector<std::shared_ptr<Command>> get_nexts() const = 0;
  virtual std::string to_cpp_string() const = 0;
  virtual ConcreteCommandType command_type() const = 0;
};

class MultiPathCommand : public Command {
 public:
  explicit MultiPathCommand() : nexts() {}
  std::vector<std::weak_ptr<Command>> nexts;
  virtual std::vector<std::shared_ptr<Command>> get_nexts() const override final {
    std::vector<std::shared_ptr<Command>> res;
    for (const auto &next : nexts) {
      res.push_back(next.lock());
    }
    return res;
  }
};

class Switch : public MultiPathCommand {
 public:
  virtual std::string to_cpp_string() const override final {
    return "  switch(stack.switch_()) {\n";
  }
  virtual std::shared_ptr<Command> exec(Stack &) const override final;
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Switch;
  }
};

class Pointer : public MultiPathCommand {
 public:
  virtual std::string to_cpp_string() const override final {
    return "  switch(stack.pointer()) {\n";
  }
  virtual std::shared_ptr<Command> exec(Stack &) const override final;
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Pointer;
  }
};

class Jez : public MultiPathCommand {
 public:
  virtual std::string to_cpp_string() const override final {
    return "  switch(stack.eq_zero()) {\n";
  }
  virtual std::shared_ptr<Command> exec(Stack &) const override final;
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Jez;
  }
};

class Halt : public Command {
 public:
  Halt() {}
  virtual std::shared_ptr<Command> exec(Stack &) const override final {
    return std::shared_ptr<Command>();
  }
  virtual std::vector<std::shared_ptr<Command>> get_nexts() const override final {
    return std::vector<std::shared_ptr<Command>>();
  }
  virtual std::string to_cpp_string() const override final {
    return "  std::exit(0);\n";
  }
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Halt;
  }
};

class SinglePathCommand : public Command {
 public:
  SinglePathCommand() : next() {};
  explicit SinglePathCommand(const std::weak_ptr<Command> &next)
    : next(next) {}
  std::weak_ptr<Command> next;
  virtual std::vector<std::shared_ptr<Command>> get_nexts() const override final {
    return std::vector<std::shared_ptr<Command>>(1, next.lock());
  }
};

class Nop : public SinglePathCommand {
 public:
  Nop() : SinglePathCommand() {}
  virtual std::shared_ptr<Command> exec(Stack &) const override final {
    return next.lock();
  }
  virtual std::string to_cpp_string() const override final {
    return "";
  }
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Nop;
  }
};

class Push : public SinglePathCommand {
 public:
  explicit Push(int value) : SinglePathCommand(), value(value) {}
  std::shared_ptr<Command> exec(Stack &) const override final;
  virtual std::string to_cpp_string() const override final {
    return "  stack.push(" + std::to_string(value) + ");\n";
  }
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Push;
  }
  int32_t get_value() const { return value; }
 private:
  int value;
};

class PushArray : public SinglePathCommand {
 public:
  explicit PushArray(const std::vector<int32_t> &ary)
    : SinglePathCommand(), data(ary) {}
  std::shared_ptr<Command> exec(Stack &) const override final;
  virtual std::string to_cpp_string() const override final {
    std::stringstream ss;
    ss << "  {\n";
    ss << "    std::vector<int32_t> ary = {\n";
    for (size_t i = 0; i < data.size(); ++i) {
      ss << "      " << data[i] << ((i == data.size() - 1) ? "" : ",") << "\n";
    }
    ss << "    };\n";
    ss << "    stack.push_array(ary);\n";
    ss << "  }\n";
    return ss.str();
  }
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::PushArray;
  }
 private:
  std::vector<int32_t> data;
};

class Duplicate : public SinglePathCommand {
 public:
  Duplicate() : SinglePathCommand() {}
  std::shared_ptr<Command> exec(Stack &) const override final;
  virtual std::string to_cpp_string() const override final {
    return "  stack.duplicate();\n";
  }
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Duplicate;
  }
};

class InNumber : public SinglePathCommand {
 public:
  InNumber() : SinglePathCommand() {}
  std::shared_ptr<Command> exec(Stack &) const override final;
  virtual std::string to_cpp_string() const override final {
    return "  stack.push(get_number());\n";
  }
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::InNumber;
  }
};

class InChar : public SinglePathCommand {
 public:
  InChar() : SinglePathCommand() {}
  std::shared_ptr<Command> exec(Stack &) const override final;
  virtual std::string to_cpp_string() const override final {
    return "  stack.push(get_char());\n";
  }
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::InChar;
  }
};

class Pop : public SinglePathCommand {
 public:
  Pop() : SinglePathCommand(), count(1) {}
  Pop(int32_t count) : SinglePathCommand(), count(count) {}
  std::shared_ptr<Command> exec(Stack &) const override final;
  virtual std::string to_cpp_string() const override final {
    std::stringstream ss;
    ss << "  for (int32_t i = 0; i < " << count << "; ++i) {\n";
    ss << "    stack.pop();\n";
    ss << "  }\n";
    return ss.str();
  }
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Pop;
  }
 private:
  int32_t count;
};

class OutNumber : public SinglePathCommand {
 public:
  OutNumber() : SinglePathCommand() {}
  std::shared_ptr<Command> exec(Stack &) const override final;
  virtual std::string to_cpp_string() const override final {
    return "  stack.out_number();\n";
  }
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::OutNumber;
  }
};

class OutChar: public SinglePathCommand {
 public:
  OutChar() : SinglePathCommand() {}
  std::shared_ptr<Command> exec(Stack &) const override final;
  virtual std::string to_cpp_string() const override final {
    return "  stack.out_char();\n";
  }
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::OutChar;
  }
};

class BinaryOp : public SinglePathCommand {
 public:
  std::shared_ptr<Command> exec(Stack &) const noexcept override final;
 private:
  virtual int bin_op(int, int) const = 0;
};

class Add : public BinaryOp {
 public:
  virtual std::string to_cpp_string() const override final {
    return "  stack.add();\n";
  }
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Add;
  }
 private:
  virtual int bin_op(int, int) const noexcept final;
};

class Subtract : public BinaryOp {
 public:
  virtual std::string to_cpp_string() const override final {
    return "  stack.sub();\n";
  }
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Subtract;
  }
 private:
  virtual int bin_op(int, int) const noexcept final;
};

class Multiply : public BinaryOp {
 public:
  virtual std::string to_cpp_string() const override final {
    return "  stack.mul();\n";
  }
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Multiply;
  }
 private:
  virtual int bin_op(int, int) const noexcept final;
};

class Divide : public BinaryOp {
 public:
  virtual std::string to_cpp_string() const override final {
    return "  stack.div();\n";
  }
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Divide;
  }
 private:
  virtual int bin_op(int, int) const final;
};

class Modulo : public BinaryOp {
 public:
  virtual std::string to_cpp_string() const override final {
    return "  stack.mod();\n";
  }
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Modulo;
  }
 private:
  virtual int bin_op(int, int) const final;
};

class Greater : public BinaryOp {
 public:
  virtual std::string to_cpp_string() const override final {
    return "  stack.greater();\n";
  }
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Greater;
  }
 private:
  virtual int bin_op(int, int) const noexcept final;
};

class Not : public SinglePathCommand {
 public:
  virtual std::string to_cpp_string() const override final {
    return "  stack.not_();\n";
  }
  virtual std::shared_ptr<Command> exec(Stack &) const override final;
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Not;
  }
};

class Swap: public SinglePathCommand {
 public:
  virtual std::string to_cpp_string() const override final {
    return "  stack.swap();\n";
  }
  Swap() : SinglePathCommand() {}
  virtual std::shared_ptr<Command> exec(Stack &) const override final;
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Swap;
  }
};

class Roll: public SinglePathCommand {
 public:
  virtual std::string to_cpp_string() const override final {
    return "  stack.roll();\n";
  }
  Roll() : SinglePathCommand() {}
  virtual std::shared_ptr<Command> exec(Stack &) const override final;
  virtual ConcreteCommandType command_type() const override final {
    return ConcreteCommandType::Roll;
  }
};

class CommandGraph {
 public:
  explicit CommandGraph(const ColorBlockGraph &);
  explicit CommandGraph(const pas::PAS &);
  void exec() const;
  std::shared_ptr<Command> root() const { return nodes.front(); }
 private:
  std::vector<std::shared_ptr<Command>> nodes;
};
