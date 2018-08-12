#include "interpret.hpp"
#include <stdexcept>
#include <map>
#include <iostream>
#include "io32.hpp"
#include "parser.hpp"

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

std::shared_ptr<Command> Switch::exec(Stack & stack) const {
  //std::cerr << "Switch" << std::endl;
  if (!stack.empty()) {
    int32_t value;
    value = stack.top();
    stack.pop();
    int32_t m = mod(value, 2);
    return nexts[m].lock();
  } else {
    return nexts[0].lock();
  }
}

std::shared_ptr<Command> Pointer::exec(Stack & stack) const {
  //std::cerr << "Pointer" << std::endl;
  if (!stack.empty()) {
    int32_t value;
    value = stack.top();
    stack.pop();
    int32_t m = mod(value, 4);
    return nexts[m].lock();
  } else {
    return nexts[0].lock();
  }
}

std::shared_ptr<Command> Jez::exec(Stack & stack) const {
  //std::cerr << "Jez" << std::endl;
  if (!stack.empty()) {
    int32_t value;
    value = stack.top();
    stack.pop();
    if (value == 0) {
      return nexts[1].lock();
    } else {
      return nexts[0].lock();
    }
  } else {
    return nexts[0].lock();
  }
}

std::shared_ptr<Command> Push::exec(Stack & stack) const {
  //std::cerr << "Push" << std::endl;
  stack.push(value);
  return next.lock();
}

std::shared_ptr<Command> PushArray::exec(Stack & stack) const {
  stack.push_array(data);
  return next.lock();
}

std::shared_ptr<Command> Duplicate::exec(Stack & stack) const {
  //std::cerr << "Duplicate" << std::endl;
  if (!stack.empty()) stack.push(stack.top());
  return next.lock();
}

std::shared_ptr<Command> InNumber::exec(Stack & stack) const {
  //std::cerr << "InNumber" << std::endl;
  int32_t value;
  std::cin >> value;
  stack.push(value);
  return next.lock();
}

std::shared_ptr<Command> InChar::exec(Stack & stack) const {
  //std::cerr << "InChar" << std::endl;
  stack.push(io32::getchar());
  return next.lock();
}

std::shared_ptr<Command> Pop::exec(Stack & stack) const {
  //std::cerr << "Pop" << std::endl;
  if (!stack.empty()) stack.pop();
  return next.lock();
}

std::shared_ptr<Command> OutNumber::exec(Stack & stack) const {
  //std::cerr << "OutNumber" << std::endl;
  if (!stack.empty()) {
    std::cout << stack.top();
    stack.pop();
  }
  return next.lock();
}

std::shared_ptr<Command> OutChar::exec(Stack & stack) const {
  //std::cerr << "OutChar" << std::endl;
  if (!stack.empty()) {
    io32::putchar(stack.top());
    stack.pop();
  }
  return next.lock();
}

std::shared_ptr<Command> BinaryOp::exec(Stack & stack) const noexcept {
  if (stack.size() >= 2) {
    int arg2 = stack.top(); stack.pop();
    int arg1 = stack.top(); stack.pop();
    try {
      int result = bin_op(arg1, arg2);
      stack.push(result);
    } catch (...) {
      stack.push(arg1);
      stack.push(arg2);
    }
  } 
  return next.lock();
}

int Add::bin_op(int lhs, int rhs) const noexcept {
  //std::cerr << "Add" << std::endl;
  return lhs + rhs;
}

int Subtract::bin_op(int lhs, int rhs) const noexcept {
  //std::cerr << "Subtract" << std::endl;
  return lhs - rhs;
}

int Multiply::bin_op(int lhs, int rhs) const noexcept {
  //std::cerr << "Multiply" << std::endl;
  return lhs * rhs;
}

int Divide::bin_op(int lhs, int rhs) const {
  //std::cerr << "Divide" << std::endl;
  return lhs / rhs;
}

int Modulo::bin_op(int lhs, int rhs) const {
  //std::cerr << "Modulo" << std::endl;
  return lhs % rhs;
}

int Greater::bin_op(int lhs, int rhs) const noexcept {
  //std::cerr << "Greater" << std::endl;
  return lhs > rhs ? 1 : 0;
}

std::shared_ptr<Command> Not::exec(Stack & stack) const {
  //std::cerr << "Not" << std::endl;
  if (!stack.empty()) {
    int32_t top = stack.top(); stack.pop();
    stack.push(top ? 0 : 1);
  }
  return next.lock();
}

std::shared_ptr<Command> Swap::exec(Stack & stack) const {
  //std::cerr << "Swap" << std::endl;
  if (stack.size() >= 2) {
    int arg2 = stack.top(); stack.pop();
    int arg1 = stack.top(); stack.pop();
    stack.push(arg2);
    stack.push(arg1);
  } 
  return next.lock();
}

std::shared_ptr<Command> Roll::exec(Stack & stack) const {
  //std::cerr << "Roll" << std::endl;
  if (stack.size() >= 2) {
    int iter = stack.top(); stack.pop();
    int depth = stack.top(); stack.pop();
    if (depth >= 0 && stack.size() >= (size_t)depth) {
      if (depth > 0) {
        stack.roll(depth, mod(iter, depth));
      }
    } else {
      stack.push(depth);
      stack.push(iter);
    }
  } 
  return next.lock();
}

CommandGraph::CommandGraph(const ColorBlockGraph &graph) : nodes() {
  size_t size = graph.size();
  for (size_t i = 0; i < size; ++i) {
    for (size_t j = 0; j < 4; ++j) {
      for (size_t k = 0; k < 2; ++k) {
        int32_t index;
        size_t dp, cc;
        bool conn;
        int old_dp = j, old_cc = k;
        for (size_t l = 0; l < 4; ++l) {
          std::tie(index, dp, cc, conn, std::ignore) = graph[i].get_next_block(old_dp, old_cc);
          if (index >= 0 || conn) break;
          old_cc = 1-old_cc;
          std::tie(index, dp, cc, conn, std::ignore) = graph[i].get_next_block(old_dp, old_cc);
          if (index >= 0 || conn) break;
          old_dp += 1;
          old_dp %= 4;
        }
        if (index >= 0 && conn) {
          switch (color_to_command(graph[i].get_color(), graph[index].get_color())) {
            case CommandType::NOP:
              nodes.push_back(std::make_shared<Nop>());
              break;
            case CommandType::PUSH:
              nodes.push_back(std::make_shared<Push>(graph[i].get_size()));
              break;
            case CommandType::POP:
              nodes.push_back(std::make_shared<Pop>());
              break;
            case CommandType::ADD:
              nodes.push_back(std::make_shared<Add>());
              break;
            case CommandType::SUBTRACT:
              nodes.push_back(std::make_shared<Subtract>());
              break;
            case CommandType::MULTIPLY:
              nodes.push_back(std::make_shared<Multiply>());
              break;
            case CommandType::DIVIDE:
              nodes.push_back(std::make_shared<Divide>());
              break;
            case CommandType::MOD:
              nodes.push_back(std::make_shared<Modulo>());
              break;
            case CommandType::NOT:
              nodes.push_back(std::make_shared<Not>());
              break;
            case CommandType::GREATER:
              nodes.push_back(std::make_shared<Greater>());
              break;
            case CommandType::POINTER:
              nodes.push_back(std::make_shared<Pointer>());
              break;
            case CommandType::SWITCH:
              nodes.push_back(std::make_shared<Switch>());
              break;
            case CommandType::DUPLICATE:
              nodes.push_back(std::make_shared<Duplicate>());
              break;
            case CommandType::ROLL:
              nodes.push_back(std::make_shared<Roll>());
              break;
            case CommandType::INN:
              nodes.push_back(std::make_shared<InNumber>());
              break;
            case CommandType::INC:
              nodes.push_back(std::make_shared<InChar>());
              break;
            case CommandType::OUTN:
              nodes.push_back(std::make_shared<OutNumber>());
              break;
            case CommandType::OUTC:
              nodes.push_back(std::make_shared<OutChar>());
              break;
            case CommandType::HALT:
              nodes.push_back(std::make_shared<Halt>());
              break;
            default: throw std::domain_error("Unknown CommandType");
          }
        } else if (index >= 0) {
          nodes.push_back(std::make_shared<Nop>());
        } else {
          nodes.push_back(std::make_shared<Halt>());
        }
      }
    }
  }
  for (size_t i = 0; i < size; ++i) {
    for (size_t j = 0; j < 4; ++j) {
      for (size_t k = 0; k < 2; ++k) {
        int32_t nindex;
        size_t dp, cc;
        bool conn;
        int old_dp = j, old_cc = k;
        for (size_t l = 0; l < 4; ++l) {
          std::tie(nindex, dp, cc, conn, std::ignore) = graph[i].get_next_block(old_dp, old_cc);
          if (nindex >= 0 || conn) break;
          old_cc = 1-old_cc;
          std::tie(nindex, dp, cc, conn, std::ignore) = graph[i].get_next_block(old_dp, old_cc);
          if (nindex >= 0 || conn) break;
          old_dp += 1;
          old_dp %= 4;
        }
        int index = i*8 + j*2 + k;
        int next_index = nindex*8 + dp*2 + cc;
        if (nindex >= 0 && conn) {
          switch (color_to_command(graph[i].get_color(), graph[nindex].get_color())) {
            case CommandType::NOP:
            case CommandType::PUSH:
            case CommandType::POP:
            case CommandType::ADD:
            case CommandType::SUBTRACT:
            case CommandType::MULTIPLY:
            case CommandType::DIVIDE:
            case CommandType::MOD:
            case CommandType::NOT:
            case CommandType::GREATER:
            case CommandType::DUPLICATE:
            case CommandType::ROLL:
            case CommandType::INN:
            case CommandType::INC:
            case CommandType::OUTN:
            case CommandType::OUTC:
              dynamic_cast<SinglePathCommand *>(nodes[index].get())->next = nodes[next_index];
              break;
            case CommandType::POINTER:
              {
                dynamic_cast<MultiPathCommand *>(nodes[index].get())->nexts.emplace_back(nodes[next_index]);
                int next_index2 = nindex*8 + ((dp+1)%4)*2 + cc;
                dynamic_cast<MultiPathCommand *>(nodes[index].get())->nexts.emplace_back(nodes[next_index2]);
                int next_index3 = nindex*8 + ((dp+2)%4)*2 + cc;
                dynamic_cast<MultiPathCommand *>(nodes[index].get())->nexts.emplace_back(nodes[next_index3]);
                int next_index4 = nindex*8 + ((dp+3)%4)*2 + cc;
                dynamic_cast<MultiPathCommand *>(nodes[index].get())->nexts.emplace_back(nodes[next_index4]);
              }
              break;
            case CommandType::SWITCH:
              {
                dynamic_cast<MultiPathCommand *>(nodes[index].get())->nexts.emplace_back(nodes[next_index]);
                int next_index2 = nindex*8 + dp*2 + (cc+1)%2;
                dynamic_cast<MultiPathCommand *>(nodes[index].get())->nexts.emplace_back(nodes[next_index2]);
              }
              break;
            case CommandType::HALT:;
          }
        } else if (nindex >= 0) {
          dynamic_cast<SinglePathCommand *>(nodes[index].get())->next = nodes[next_index];
        }
      }
    }
  }
}

CommandGraph::CommandGraph(const pas::PAS &pas) : nodes() {
  using namespace pas;
  std::map<std::string, int> labels;
  for (auto pasc : pas.commands) {
    switch (pasc.inst) {
      case PASCommandType::PUSH:
        nodes.push_back(std::make_shared<Push>(pasc.arg1));
        break;
      case PASCommandType::DUP:
        nodes.push_back(std::make_shared<Duplicate>());
        break;
      case PASCommandType::ROLL:
        nodes.push_back(std::make_shared<Roll>());
        break;
      case PASCommandType::INN:
        nodes.push_back(std::make_shared<InNumber>());
        break;
      case PASCommandType::INC:
        nodes.push_back(std::make_shared<InChar>());
        break;
      case PASCommandType::POP:
        nodes.push_back(std::make_shared<Pop>());
        break;
      case PASCommandType::OUTN:
        nodes.push_back(std::make_shared<OutNumber>());
        break;
      case PASCommandType::OUTC:
        nodes.push_back(std::make_shared<OutChar>());
        break;
      case PASCommandType::ADD:
        nodes.push_back(std::make_shared<Add>());
        break;
      case PASCommandType::SUB:
        nodes.push_back(std::make_shared<Subtract>());
        break;
      case PASCommandType::MUL:
        nodes.push_back(std::make_shared<Multiply>());
        break;
      case PASCommandType::DIV:
        nodes.push_back(std::make_shared<Divide>());
        break;
      case PASCommandType::MOD:
        nodes.push_back(std::make_shared<Modulo>());
        break;
      case PASCommandType::GREATER:
        nodes.push_back(std::make_shared<Greater>());
        break;
      case PASCommandType::NOT:
        nodes.push_back(std::make_shared<Not>());
        break;
      case PASCommandType::HALT:
        nodes.push_back(std::make_shared<Halt>());
        break;
      case PASCommandType::LABEL:
        nodes.push_back(std::make_shared<Nop>());
        break;
      case PASCommandType::JEZ:
        nodes.push_back(std::make_shared<Jez>());
        break;
      case PASCommandType::JMP:
        nodes.push_back(std::make_shared<Nop>());
        break;
      case PASCommandType::SWAP:
        nodes.push_back(std::make_shared<Swap>());
        break;
      default:
        throw new std::domain_error("Unknown PAS command");
    }
  }
  for (size_t i = 0; i < nodes.size() - 1; ++i) {
    switch (pas.commands[i].inst) {
      case PASCommandType::HALT: break;
      case PASCommandType::JEZ:
        dynamic_cast<MultiPathCommand *>(nodes[i].get())->nexts.emplace_back(nodes[i+1]);
        dynamic_cast<MultiPathCommand *>(nodes[i].get())->nexts.emplace_back(nodes[pas.commands[i].arg1]);
        break;
      case PASCommandType::JMP:
        dynamic_cast<SinglePathCommand *>(nodes[i].get())->next = nodes[pas.commands[i].arg1];
        break;
      default:
      dynamic_cast<SinglePathCommand *>(nodes[i].get())->next = nodes[i+1];
    }
  }
}

void CommandGraph::exec() const {
  Stack stack;
  std::shared_ptr<Command> prog_ptr = nodes.front();
  while (prog_ptr) {
    //std::cerr << (std::find(std::begin(nodes), std::end(nodes), prog_ptr) - std::begin(nodes)) << std::endl;
    //std::cerr << stack.to_s() << std::endl;
    prog_ptr = prog_ptr->exec(stack);
  }
}
