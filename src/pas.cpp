#include "pas.hpp"
#include <map>
#include <boost/optional.hpp>

namespace pas {

const std::map<std::string, PASCommandType> command_map = {
  {"PUSH",    PASCommandType::PUSH},
  {"POP",     PASCommandType::POP},
  {"ADD",     PASCommandType::ADD},
  {"SUB",     PASCommandType::SUB},
  {"MUL",     PASCommandType::MUL},
  {"DIV",     PASCommandType::DIV},
  {"MOD",     PASCommandType::MOD},
  {"NOT",     PASCommandType::NOT},
  {"GREATER", PASCommandType::GREATER},
  {"DUP",     PASCommandType::DUP},
  {"ROLL",    PASCommandType::ROLL},
  {"INN",     PASCommandType::INN},
  {"INC",     PASCommandType::INC},
  {"OUTN",    PASCommandType::OUTN},
  {"OUTC",    PASCommandType::OUTC},
  {"HALT",    PASCommandType::HALT},
  {"LABEL",   PASCommandType::LABEL},
  {"JEZ",     PASCommandType::JEZ},
  {"JMP",     PASCommandType::JMP},
  {"SWAP",    PASCommandType::SWAP},
  {"UNKNOWN", PASCommandType::UNKNOWN}
};

std::vector<std::string> split(const std::string &str, const char ch) {
  std::vector<std::string> res;
  std::string::size_type pos = str.find(ch);
  std::string::size_type old = 0;
  while (pos != std::string::npos) {
    res.push_back(str.substr(old, pos-old));
    old = pos;
    pos = str.find(ch, pos+1);
  }
  res.push_back(str.substr(old));
  return res;
}

std::vector<std::string> command_sequence(const std::string &line) {
  auto without_comment = split(line, '#');
  return split(without_comment.front(), ' ');
}

PASCommandType get_pas_command_type(const std::string &command_str) {
  auto itr = command_map.find(command_str);
  if (itr == std::end(command_map)) throw std::domain_error("Unknown PAS command: " + command_str);
  return itr->second;
}

PASCommand parse_line(const std::string &line, const std::map<std::string, int> &labels) {
  auto args = command_sequence(line);
  PASCommandType pct = get_pas_command_type(args.front());
  switch (pct) {
    case PASCommandType::LABEL:
    case PASCommandType::JEZ:
    case PASCommandType::JMP:
      return PASCommand(pct, labels.at(args.at(1)));
    case PASCommandType::PUSH:
      return PASCommand(pct, std::stoi(args.at(1)));
    default:
      return PASCommand(pct);
  }
}

boost::optional<std::string> get_label(const std::string &line) {
  auto args = command_sequence(line);
  if (args.size() < 2) return boost::none;
  if (args.front() == "LABEL") {
    return args[1];
  } else {
    return boost::none;
  }
}

PAS::PAS(const std::vector<std::string> &lines) {
  std::map<std::string, int> labels;
  for (int i = 0; i < lines.size(); ++i)
    if (const auto label_opt = get_label(lines[i]))
      labels[*label_opt] = i;
  for (const auto &line : lines) {
    commands.push_back(parse_line(line, labels));
  }
}

} // namespace pas
