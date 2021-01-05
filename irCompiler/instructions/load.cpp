#include "load.h"

#include <regex>

Load::Load(std::string const &instruction, int line)
    : Instruction(instruction, line, InstructionType::Load) {}

std::string Load::Print() const {
  std::string print = std::to_string(m_line) + " " + m_instruction + "\n" +
                      "Instruction:\t" + PrintType() + "\nSource:\t" +
                      GetSourceVariable() + "\nDestination:\t" +
                      GetDestinationVariable() + "\nVariableSize:\t" +
                      std::to_string(GetVariableSize());
  return print;
}

std::string Load::GetDestinationVariable() const {
  return std::regex_replace(m_instruction, std::regex("(%[A-z-0-9]+)(\\s=.*)"),
                            std::string("$1"));
}

std::string Load::GetSourceVariable() const {
  return std::regex_replace(m_instruction, std::regex("(.*)(%[A-z-0-9]+)(,.*)"),
                            std::string("$2"));
}

int Load::GetVariableSize() const {
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