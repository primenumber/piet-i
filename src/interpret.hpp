#pragma once
#include <memory>
#include <sstream>
#include <stack>
#include <vector>
#include <codecvt>
#include <boost/optional.hpp>
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

class Command {
 public:
  virtual std::shared_ptr<Command> exec(Stack &) const = 0;
  virtual std::vector<std::shared_ptr<Command>> get_nexts() const = 0;
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
  virtual std::shared_ptr<Command> exec(Stack &) const override final;
};

class Pointer : public MultiPathCommand {
 public:
  virtual std::shared_ptr<Command> exec(Stack &) const override final;
};

class Jez : public MultiPathCommand {
 public:
  virtual std::shared_ptr<Command> exec(Stack &) const override final;
};

class Halt : public Command {
 public:
  Halt() {}
  virtual std::shared_ptr<Command> exec(Stack &) const override final {
    //std::cerr << "Halt" << std::endl;
    return std::shared_ptr<Command>();
  }
  virtual std::vector<std::shared_ptr<Command>> get_nexts() const override final {
    return std::vector<std::shared_ptr<Command>>();
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
    //std::cerr << "Nop" << std::endl;
    return next.lock();
  }
};

class Push : public SinglePathCommand {
 public:
  explicit Push(int value) : SinglePathCommand(), value(value) {}
  std::shared_ptr<Command> exec(Stack &) const override final;
 private:
  int value;
};

class Duplicate : public SinglePathCommand {
 public:
  Duplicate() : SinglePathCommand() {}
  std::shared_ptr<Command> exec(Stack &) const override final;
};

class InNumber : public SinglePathCommand {
 public:
  InNumber() : SinglePathCommand() {}
  std::shared_ptr<Command> exec(Stack &) const override final;
};

class InChar : public SinglePathCommand {
 public:
  InChar() : SinglePathCommand() {}
  std::shared_ptr<Command> exec(Stack &) const override final;
};

class Pop : public SinglePathCommand {
 public:
  Pop() : SinglePathCommand() {}
  std::shared_ptr<Command> exec(Stack &) const override final;
};

class OutNumber : public SinglePathCommand {
 public:
  OutNumber() : SinglePathCommand() {}
  std::shared_ptr<Command> exec(Stack &) const override final;
};

class OutChar: public SinglePathCommand {
 public:
  OutChar() : SinglePathCommand() {}
  std::shared_ptr<Command> exec(Stack &) const override final;
};

class BinaryOp : public SinglePathCommand {
 public:
  std::shared_ptr<Command> exec(Stack &) const noexcept override final;
 private:
  virtual int bin_op(int, int) const = 0;
};

class Add : public BinaryOp {
 private:
  virtual int bin_op(int, int) const noexcept final;
};

class Subtract : public BinaryOp {
 private:
  virtual int bin_op(int, int) const noexcept final;
};

class Multiply : public BinaryOp {
 private:
  virtual int bin_op(int, int) const noexcept final;
};

class Divide : public BinaryOp {
 private:
  virtual int bin_op(int, int) const final;
};

class Modulo : public BinaryOp {
 private:
  virtual int bin_op(int, int) const final;
};

class Greater : public BinaryOp {
 private:
  virtual int bin_op(int, int) const noexcept final;
};

class Not : public SinglePathCommand {
 public:
  virtual std::shared_ptr<Command> exec(Stack &) const override final;
};

class Swap: public SinglePathCommand {
 public:
  Swap() : SinglePathCommand() {}
  virtual std::shared_ptr<Command> exec(Stack &) const override final;
};

class Roll: public SinglePathCommand {
 public:
  Roll() : SinglePathCommand() {}
  virtual std::shared_ptr<Command> exec(Stack &) const override final;
};

class CommandGraph {
 public:
  explicit CommandGraph(const ColorBlockGraph &);
  explicit CommandGraph(const pas::PAS &);
  void exec() const;
 private:
  std::vector<std::shared_ptr<Command>> nodes;
};
