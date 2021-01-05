#include "functionDefinition.h"

#include <regex>

FunctionDefinition::FunctionDefinition(std::string const &instruction, int line)
    : Instruction(instruction, line, InstructionType::FunctionDefinition) {}

std::string FunctionDefinition::Print() const {
  std::string print = std::to_string(m_line) + " " + m_instruction + "\n" +
                      "Instruction:\n" + PrintType() + " FunctionName:\t" +
                      GetFunctionName();
  return print;
}

std::string FunctionDefinition::GetFunctionName() const {
  return std::regex_replace(m_instruction, std::regex("(.*@)([A-z-0-9]+)(.*)"),
                            std::string("$2"));
}

FunctionDefinitionEnd::FunctionDefinitionEnd(std::string const &instruction,
                                             int line)
    : Instruction(instruction, line, InstructionType::FunctionDefinitionEnd) {}

std::string FunctionDefinitionEnd::Print() const {
  std::string print = std::to_string(m_line) + " " + m_instruction + "\n" +
                      "Instruction:\n" + PrintType();
  return print;
}