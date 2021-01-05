#include "store.h"
#include "../../common/opcode.h"
#include <regex>

Store::Store(std::string const &instruction, int line)
    : Instruction(instruction, line, InstructionType::Store) {}

int Store::GetVariableSize() const {
  int size = 0;

  std::string number = std::regex_replace(
      m_instruction, std::regex("(.*i)([0-9]+).*"), std::string("$2"));

  try {
    size = atoi(number.c_str());
  } catch (...) {
    size = 0;
  }

  return size / 8;
}

std::string Store::GetVariableName() const {
  return std::regex_replace(m_instruction, std::regex("(.*)(%[A-z-0-9]+)(,.*)"),
                            std::string("$2"));
}

std::string Store::GetVariableValue() const {
  return std::regex_replace(
      m_instruction, std::regex("(.*)([A-z-0-9]+)(,.*,.*)"), std::string("$2"));
}

std::string Store::Print() const {
  std::string print = std::to_string(m_line) + " " + m_instruction + "\n" +
                      "Instruction:\t" + PrintType() + "\nVariableName:\t" +
                      GetVariableName() + "\nVariableSize:\t" +
                      std::to_string(GetVariableSize()) + "\nVariableValue:\t" +
                      GetVariableValue();
  return print;
}
