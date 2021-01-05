#include "alloca.h"
#include "../../common/opcode.h"
#include <regex>

Alloca::Alloca(std::string const &instruction, int line)
    : Instruction(instruction, line, InstructionType::Alloca) {}

int Alloca::GetVariableSize() const {
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

std::string Alloca::GetVariableName() const {
  return std::regex_replace(m_instruction, std::regex("(.*)(%[0-9]+).*"),
                            std::string("$2"));
}

std::string Alloca::Print() const {
  std::string print = std::to_string(m_line) + " " + m_instruction + "\n" +
                      "Instruction:\n" + PrintType() + " VariableName:\t" +
                      GetVariableName() + "\tVariableSize:\t" +
                      std::to_string(GetVariableSize());
  return print;
}
