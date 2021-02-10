#include "instruction.h"

Instruction::Instruction(std::string const &instruction, int line,
                         InstructionType type)
    : m_instruction(instruction), m_line(line), m_type(type) {}

int Instruction::GetLine() const { return m_line; }

std::string Instruction::GetInstruction() const { return m_instruction; }

InstructionType Instruction::GetType() const { return m_type; }

InstructionType Instruction::GetType(std::string const &instruction) {
  if (instruction.find("alloca") != std::string::npos) {
    return InstructionType::Alloca;
  } else if (instruction.find("store") != std::string::npos) {
    return InstructionType::Store;
  } else if (instruction.find("load") != std::string::npos) {
    return InstructionType::Load;
  } else if (instruction.find("sext") != std::string::npos) {
    return InstructionType::Sext;
  } else if (instruction.find("define dso_local") != std::string::npos) {
    return InstructionType::FunctionDefinition;
  } else if (instruction == "}") {
    return InstructionType::FunctionDefinitionEnd;
  }

  return InstructionType::Unknown;
}

std::string Instruction::PrintType() const {
  switch (m_type) {
  case InstructionType::Alloca:
    return "Alloca";
  case InstructionType::Store:
    return "Store";
  case InstructionType::Load:
    return "Load";
  case InstructionType::Sext:
    return "Sext";
  case InstructionType::FunctionDefinition:
    return "FunctionDefinition";
  case InstructionType::FunctionDefinitionEnd:
    return "FunctionDefinitionEnd";
  default:
    break;
  }
  return "Unknown";
}